/*
 * soilModBus.c
 *
 *  Created on: 19 abr 2026
 *      Author: vicks
 *      ABOUT: MEF para Sensor de Suelo con MODBUS
 *      Estados: INIT → IDLE → REQUESTING → WAITING → PARSING → DONE
 *                                           → ERROR
 *
 *   Trama de solicitud (Modbus FC03, 3 registros desde 0x0000):
 *   [ADDR] [FC]   [REG_H] [REG_L] [QTY_H] [QTY_L] [CRC_L] [CRC_H]
 *   [0x01] [0x03] [0x00]  [0x00]  [0x00]  [0x03]  [0x05]  [0xCB]
 *
 *   Trama de respuesta esperada (11 bytes):
 *   [ADDR][FC][BYTE_CNT][HUM_H][HUM_L][TEMP_H][TEMP_L][EC_H][EC_L][CRC_L][CRC_H]
 */

#include "soilModBus.h"
#include "uart_rs485.h"
#include "delay.h"

/*Constantes*/
#define SLAVE_ADDRESS 0x01
#define RESPONSE_LEN 11 //Longitud de la respuesta esperada
#define TIMEOUT 500
#define REQUEST_CMD 0x03
#define REG_H 0x00
#define REG_L 0x00
#define QTY_H 0x00
#define QTY_L 0x03
#define CRC_L 0x05
#define CRC_H 0xCB


/*Trama para leer 3 registros desde 0x0000 el sensor con ModBus*/

static const uint8_t REQUEST_FRAME[] = {
    SLAVE_ADDRESS, REQUEST_CMD, REG_H, REG_L, QTY_H, QTY_L, CRC_L,CRC_H
};



/*Variables privadas*/

static MEFSensorState_t soilsensorstate=IDLE485; // Variable que almacena los esdtados de la mef
static uint8_t rxBuffer[RESPONSE_LEN];  // Buffer de la respuesta del sensor
static uint8_t rxIdx=0;
static soilsensor_data_t soilData={0};
static bool_t dataValid =false;
static delay_t rxTimeout; // para el retardo no bloqueante del timeout de recepcion




bool_t requestSoilData(){
	if(soilsensorstate ==IDLE485 ||
	   soilsensorstate == DONE485)
	{
	soilsensorstate=REQUESTING485;
	return true;
	}
	return false;
}

void startSoilSensor(){
	rxIdx = 0;
	soilsensorstate=INIT485;

}
/*
 * Funcion que parsea el buffer rxBuf y rellena soilData*/
static bool parseResponse(void)
{
	//Verificacion de que el buffer esté completo antes de parsear
	if (rxIdx < RESPONSE_LEN) {
		return false;
	}
	//Validación de la respuesta esperada
	if(rxBuffer[0] != SLAVE_ADDRESS || rxBuffer[1]!=0x03|| rxBuffer[2]!=0x06){
		return false;
	}

		/*si el dato es válido reconstruyo el numero de 16bits
		 * con los dos bytes que me devuelve el sensor */
		uint16_t humRaw = ((uint16_t)rxBuffer[3]<<8) | rxBuffer[4];
		uint16_t tempRaw =((uint16_t)rxBuffer[5]<<8) | rxBuffer[6];
		uint16_t ec_raw =((uint16_t)rxBuffer[7]<<8)| rxBuffer[8] ;

		/*Una vez que tengo los datos en 16 bits
		 * se adecua para que tenga sentido físico*/

		soilData.humedad = humRaw/10.0f;
		soilData.temperatura = tempRaw/10.0f;
		soilData.ec= (float)ec_raw;

		return true;
	}

/* Devuelve el estado actual de la MEF del sensor.
 * Cuando está en DONE485 lo regresa a IDLE para que
 * la próxima llamada a requestSoilData() funcione. */

MEFSensorState_t requestSoilDataState(){
	MEFSensorState_t sensorstate;
	sensorstate=soilsensorstate;
	if (sensorstate==DONE485){
		soilsensorstate=IDLE485;
	}
	return sensorstate;
}


void soilSensorInit(){
	uart485Init();
	HAL_Delay(500);
	dataValid=false;
	rxIdx=0;

}


// getter devuelve puntero a datos validos o null si no hay datos
const soilsensor_data_t * soilSensorGetData(void){
	if(dataValid){
		return &soilData;
	}
	return NULL;
}

void MEF_soilSensorUpdate(){
	switch(soilsensorstate){
	case INIT485:
		soilSensorInit();
		//uartSendString((uint8_t*)"485 iniciado\r\n");
		soilsensorstate=IDLE485;
		break;
	case IDLE485:
		// no se hace nada por ahora porque el cambio viene de afuera, se puede poner algo, pero creo q mejor no
		break;
	case REQUESTING485:

		//Pone DE y RE en alto y Envía la trama para solicitar el dato al sensor
		//uartSendString((uint8_t*)"me pidieron un dato\r\n");

		if (uart485Transmit(REQUEST_FRAME,sizeof(REQUEST_FRAME))==RS485_OK){
			/*Inicio el contador de tiempo (delay no bloqueante) para esperar la respuesta*/
			//uartSendString((uint8_t*)"lo solicite al sensor\r\n");
			delayInit(&rxTimeout,TIMEOUT);
			rxIdx = 0;

			soilsensorstate=WAITING485;
			//enableReceive();

		}
		else{
			soilsensorstate=ERROR485;
			//uartSendString((uint8_t*)"oops tuve un error al ller el sensor\r\n");
		}

	break;
	case WAITING485:
	{
		//Pone DE y RE en bajo y consulta si hay algo rebido en la uart1, sale a PARSING si recibe y si timeout va a a error y setea un error code

		uint8_t byte;
		//comienzo leer byte a byte hasta el final
		while(rxIdx<RESPONSE_LEN)
		{
			rs485status_t status= uart485ReceiveByte(&byte);
			if(status == RS485_OK)
			{
				//Significa que estoy recibiendo bytes aun y los voy guardando en el buffer
				rxBuffer[rxIdx++]=byte;
				//uartSendString((uint8_t*)"recibi un byte\r\n");

			}else{
				//No hay mas datos para leer
				break;
			}
		}
		if(rxIdx>=RESPONSE_LEN){
			/*Trama completa, debo ir a parsear*/
			soilsensorstate= PARSING485;
			//uartSendString((uint8_t*)"ya toca ir al parseo\r\n");

		} else if (delayRead(&rxTimeout)){
			//delayRead me va a devolver true la primera vex que se haya cumplido el tiempo de timeout
			//entonces si no termine de leer la trama y paso el tiempo, estoy en un error de timeout
			soilsensorstate= ERROR485;
		}
		//Si la trama no está completa pero no timeout, vuelvo en el próximo ciclo a intentar seguir armando el buffer
		break;
	}
	case PARSING485:
		//uartSendString((uint8_t*)"entre al parseo\r\n");
		//llama a la función de parseo que verifica el header primero, devuelve true si tiene data valida
		if(parseResponse())
		{
			//uartSendString((uint8_t*)"recibi bien el dato\r\n");
			dataValid=true;
			soilsensorstate = DONE485;
		}else {
			soilsensorstate=ERROR485;
			//uartSendString((uint8_t*)"el dato estaba corrupto\r\n");
		}
		break;
	case DONE485:
		//uartSendString((uint8_t*)"aca esperando que lean el dato\r\n");
		//cuando esta done, es como una bandera y permite acceder a los datos
		//se puede agregar un parpadeo característico
		break;
	case ERROR485:
		//uartSendString((uint8_t*)"algo salio mal\r\n");
		//retorna el error almacena de donde vino
		soilsensorstate= INIT485;
		break;
	default:
		break;

	}
}

