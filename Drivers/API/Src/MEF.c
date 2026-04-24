/*
 * MEF.c
 *
 * Maquina de estados principal del sistema de medición de suelos.
 *  Flujo de estados:
 *  INIT → IDLE → REQ_SENSING → SENSING → SHOW → IDLE
 *                         → ERROR1 → INIT
 *  Salidas:
 *    - UART2  (debug / terminal serie)
 *    - LCD 16x2 por I2C
 *
 *  Created on: 11 abr 2026
 *      Author: vicks
 */

#include "MEF.h"

/*Tiempos en milisegundos*/
#define WAITTIME 3000  //tiempo de inicializacion

#define WAITTIME2 5000 //tiempo durante el que se muestran los datos en LCD
#define TOGGLESENSING 500
#define TOGGLEIDLE 2000

/*Longitud máxima del buffer de mensajes para UART*/
#define MSG_LEN 80

/*Variables privadas de la MEF*/
static tick_t time=0;
static bool_t button=false;
static bool_t flag=false;
static MEFState_t mef = IDLE;
static MEFSensorState_t soilstate;

/*buffer para armar el mensaje de resultado*/
static char buffer[MSG_LEN];


/*Mostrar datos por UART2 y LCD
 * utilizo const para que los datos no puedan ser modificados dentro de la funcion
 * */

static void showSensorData (const soilsensor_data_t *d){

	char lcdLine [17]; //16 caracteres + \0

	//envio de datos por UART para debug
	sprintf(buffer, "Humedad: %.1f % \r\n", d->humedad);
	uartSendString((uint8_t*)buffer);

	sprintf(buffer, "Temperatura: %.1f C\r\n", d->temperatura);
	uartSendString((uint8_t*)buffer);

	sprintf(buffer, "EC: %.0f uS/cm \r\n", d->ec);
	uartSendString((uint8_t*)buffer);

	uartSendString((uint8_t*)"Medición Finalizada\r\n");

	//envio de datos al LCD

	//Linea 1: Temperatura y humedad
	sprintf(lcdLine, "H:%.1f%%  T:%.1fC", d->humedad, d->temperatura);
	LCD_writeStringF1(lcdLine);

	//linea 2: EC
	sprintf(lcdLine,"EC:%.0f uS/cm", d->ec);
	LCD_writeStringF2(lcdLine);



}

void MEF_init(void) {
	//uartInit();

	mef = INIT;

}




void MEFUpdate (void) {

	MEF_soilSensorUpdate(); //actualiza la submef del sensor

	switch(mef){
	case INIT:
		// agregar la inicializacion del lcd y el envio del mensaje por LCD
		LCD_init();
		uartSendString((uint8_t*)"Bienvenidos al sistema de Medicion de Suelos\r\n");
		time=getTick();

		LCD_writeStringF1("Iniciando SMS");
		LCD_writeStringF2("Aguarde . . .");
		soilSensorInit();

		flag =false;
		mef=IDLE;
		break;

	case IDLE:
		button=readKey();
		if (button==true)
			{
				mef=REQ_SENSING;
				button=false;
				break;
			}
		if((getTick() - time) >= WAITTIME){
			if (flag==false){
			uartSendString((uint8_t*)"Presione el pulsador para iniciar\r\n");
			LCD_writeStringF1("Presione el");
			LCD_writeStringF2("  Pulsador  ");
			//enviar mensaje por LCD tmb
			flag=true;
			ledToggle(TOGGLEIDLE);
				}

			}
		break;


	case REQ_SENSING:
		ledToggle(TOGGLESENSING);
	    uartSendString((uint8_t*)"Iniciando medicion...\r\n");
	    LCD_writeStringF1("Midiendo");
	    LCD_writeStringF2("Aguarde . . .");
	    if(requestSoilData()){
	        flag = false;
	        mef = SENSING;
	        uartSendString((uint8_t*)"Comence la medicion\r\n");
	    } else {
	        uartSendString((uint8_t*)"Esperando sensor \r\n");
	    }
	    break;


	case SENSING:
		soilstate= requestSoilDataState();
		if(!flag){
			uartSendString((uint8_t*)"Estoy en sensing\r\n");

		flag=true;
		}


		if (soilstate==ERROR485){
			uartSendString((uint8_t*)"ERROR Obteniendo datos del sensor de suelo\r\n");
			//Agregar el envio de datos al LCD
			mef=ERROR1;
			break;
		}
		if(soilstate==DONE485){
			mef=SHOW;
			ledOff();
			time=getTick();
			flag=false;
		}
		/*si está en otro estado: waiting, requesting, parsing. chequea en el siguiente bucle*/

		break;


	case SHOW:
	{
	  if(!flag) {
	      const soilsensor_data_t *data =soilSensorGetData();
	      if (data != NULL) {
	          showSensorData(data);
	      } else {
	          uartSendString((uint8_t*)
	          "Datos no disponibles\r\n");

	      }
	      flag = true;
	  }

	  //Delay no bloqueante para mostrar los datos en el lcd durante el WAITTIME2
	  if((getTick()-time)>=WAITTIME2){
	      flag=false;
	      mef=IDLE;
	  }
	  break;
	}
	case ERROR1:
		uartSendString((uint8_t*)"Error en la medicion, reiniciando\r\n");
		LCD_writeStringF1("ERROR . . .");
		LCD_writeStringF2("Reiniciando . . .");

		mef=INIT;
		break;
	default:
		uartSendString((uint8_t*)"Error desconocido, reiniciando\r\n");
		LCD_writeStringF1("ERROR . . .");
		LCD_writeStringF2("Reiniciando . . .");

		mef=INIT;
		break;
	}
	}

