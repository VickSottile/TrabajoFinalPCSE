/*
 * API_debounce.h
 *
 *  Created on: 30 mar 2026
 *      Author: vicks
 */



#ifndef API_INC_API_DEBOUNCE_H_
#define API_INC_API_DEBOUNCE_H_

#include "delay.h"
#include "types.h"
#include "uart.h"
#include "board_port.h"



// Nuevo tipo de datos enumerado llamado estadoMEF_t
typedef enum{
    BUTTON_UP,
	BUTTON_FALLING,
	BUTTON_DOWN,
	BUTTON_RISSING
} debounceState_t;

 void debounceFSM_init();		// debe cargar el estado inicial
 void debounceFSM_update();	// debe leer las entradas, resolver la lógica de transición de estados y actualizar las salidas

bool_t readKey(); /* La función readKey debe leer una variable interna del módulo y devolver true o false si la tecla fue presionada.  Si devuelve true, debe resetear (poner en false) el estado de la variable.*/



#endif /* API_INC_API_DEBOUNCE_H_ */
