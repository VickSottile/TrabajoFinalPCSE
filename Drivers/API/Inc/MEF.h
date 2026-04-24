/*
 * MEF.h
 *
 *  Created on: 11 abr 2026
 *      Author: vicks
 */

#ifndef API_INC_MEF_H_
#define API_INC_MEF_H_
#include <debounce.h>
#include <stdbool.h>
#include <stdio.h>
#include "board_port.h"
#include "soilModBus.h"
#include "lcd.h"




typedef enum{
    INIT=0,
	IDLE,
	REQ_SENSING,
	SENSING,
	SHOW,
	ERROR1
} MEFState_t;

/* Actualización de la máquina de estados*/
void MEFUpdate (void);

/*Inicializacion de la MEF*/
void MEF_init(void);

/*Setter para saber cuando tengo un dato disponible*/
void MEF_setDataReady (void);

#endif /* API_INC_MEF_H_ */
