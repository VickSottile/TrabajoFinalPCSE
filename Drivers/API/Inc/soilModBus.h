/*
 * soilModBus.h
 *
 *  Created on: 19 abr 2026
 *      Author: vicks
 */

#ifndef API_INC_SOILMODBUS_H_
#define API_INC_SOILMODBUS_H_

#include "uart_rs485.h"
#include "board_port.h"
#include "uart.h"



typedef enum{
    INIT485=0,
	IDLE485,
	REQUESTING485,
	WAITING485,
	PARSING485,
	DONE485,
	ERROR485
	 } MEFSensorState_t;

typedef struct {
	float temperatura; /*°C*/
	float humedad; /* % */
	float ec; /* uS/cm */
}soilsensor_data_t;

void soilSensorInit(void);
void MEF_soilSensorUpdate(void);
bool_t requestSoilData(void); // Setter para solicitar la medición
void startSoilSensor(void); //Setter del primer estado de la maquina de estados
MEFSensorState_t requestSoilDataState(); //Getter del estado de la MEF del Sensor
const soilsensor_data_t * soilSensorGetData(void);// getter devuelve puntero a datos validos o null si no hay datos

#endif /* API_INC_SOILMODBUS_H_ */
