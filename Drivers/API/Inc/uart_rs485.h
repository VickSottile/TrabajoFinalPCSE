/*
 * uart_rs485.h
 *
 *  Created on: 19 abr 2026
 *      Author: vicks
 *       About: Capa de abstracción de hardware RS485
 */

#ifndef API_INC_UART_RS485_H_
#define API_INC_UART_RS485_H_

#include "stm32f4xx_hal.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "string.h"
#include "board_port.h"

typedef enum{
	RS485_OK=0,
	RS485_ERROR,
	RS485_TIMEOUT,
	RS485_BUSY
}rs485status_t;

typedef bool bool_t;



bool_t uart485Init(void); // inicializa la uart para 485
rs485status_t uart485SendString(uint8_t * pstring); //envía un strig
void uart485ReceiveStringSize(uint8_t * pstring, uint16_t size); // recibe un string y su tamaño
bool_t uart485DataAvailable(void); //devuelve true si hay un dato disponible para leer
void enableTransmit(void);
void enableReceive(void);
rs485status_t uart485Transmit(const uint8_t *data, uint16_t len);
rs485status_t uart485ReceiveByte(uint8_t *byte); // intenta recibir un byte con timeout de corto duvuelve RS485_OK si llegó un byte, RS485_TIMEOUT si no había dato aun

#endif /* API_INC_UART_RS485_H_ */
