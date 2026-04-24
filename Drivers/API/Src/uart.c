/*
 * API_uart.c
 *
 *  Created on: 2 abr 2026
 *      Author: vicks
 */
#include <uart.h>

#define TIMEOUT_UART2 10
static UART_HandleTypeDef huart2;

static HAL_StatusTypeDef transmitido;

static  bool_t uartRxReady=false; // variable interna que refleja si hay datos para leer

//Funciones
bool_t uartInit(){ //inicializa la UART
	  huart2.Instance = USART2;
	  huart2.Init.BaudRate = 115200;
	  huart2.Init.WordLength = UART_WORDLENGTH_8B;
	  huart2.Init.StopBits = UART_STOPBITS_1;
	  huart2.Init.Parity = UART_PARITY_NONE;
	  huart2.Init.Mode = UART_MODE_TX_RX;
	  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
	  if (HAL_UART_Init(&huart2) != HAL_OK)
	  {
		  Error_Handler();
		  return false;

	  } else {
		    char buffer[200];

		    //utilizo la funcion sprintf que me permite armar el buffer para enviar
		    //transformando todo a un string
		    sprintf(buffer,
		        "UART Config:\r\n"
		        "BaudRate: %lu\r\n"
		        "WordLength: %d\r\n"
		        "StopBits: %d\r\n"
		        "Parity: %d\r\n",
		        huart2.Init.BaudRate,
		        huart2.Init.WordLength,
		        huart2.Init.StopBits,
		        huart2.Init.Parity
		    );

		    uartSendStringSize((uint8_t*)buffer, strlen(buffer)); // funciona ok
		  return true;

	  }

}
void uartSendString(uint8_t *pstring) //envia el  String
{
	if (pstring!=NULL)
	{
		/* Verifica que la longitud no sea cero*/
		uint16_t len =(uint16_t)strlen((char*)pstring);
		if (len<1){
			return;
		}
		transmitido = HAL_UART_Transmit(&huart2, pstring, strlen((char*)pstring),TIMEOUT_UART2);
		if (transmitido != HAL_OK)
			{
				Error_Handler();
				return;
			}

	}

		return;
}
void uartSendStringSize(uint8_t * pstring, uint16_t size) {//envía el tamaño del string
	//size entre 1 y 256 (valor razonable que se puede ajustar)
	if ((size>=1 && size<=256)&&(pstring!=NULL))
	{
	transmitido = HAL_UART_Transmit(&huart2, pstring,size,TIMEOUT_UART2);

	if (transmitido != HAL_OK)
	{
		Error_Handler();
	}
	}
	//Se podria implementar un parpadeo de leds con codigo de error
	return;
}
void uartReceiveStringSize(uint8_t * pstring, uint16_t size) // Recibe el tamaño del string
{
	uartRxReady = false;
	if (pstring!=NULL)
		{
		transmitido =HAL_UART_Receive (&huart2, pstring, size,TIMEOUT_UART2);
		if (transmitido== HAL_ERROR){
			Error_Handler();
		}
		if (transmitido == HAL_TIMEOUT){
			uartRxReady = false; // Saque el errorhandler porque se colgaba al llamarlo por timeout
		}else{
			uartRxReady = true; // significa que hay datos recibidos disponibles
		}
		}
	return;
}

//funcion que permite leer la variable static uartRxReady que indica si hay datos para leer
bool_t uartDataAvailable(void)
{
    return uartRxReady;
}
