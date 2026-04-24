/*
 * I2C_port.c
 *
 *  Created on: 13 abr 2026
 *      Author: vicks
 */

#include <lcd_I2C_port.h>
#define ADDRESS 0x27 //mejora posible: podria ser una variable static y que se cambie con un setter para otros modulos



//static uint8_t timeD;

static I2C_HandleTypeDef hi2c1;
static HAL_StatusTypeDef status;

void I2Cinit() {

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

 void I2CWriteByte(uint8_t data)
{
	status = HAL_I2C_Master_Transmit(&hi2c1, ADDRESS <<1, &data, 1, HAL_MAX_DELAY);
	if (status!=HAL_OK){
		Error_Handler();
	}
	return;

}

 uint8_t I2CReadByte(){
	 uint8_t receivedByte=0;

	status= HAL_I2C_Master_Receive(&hi2c1, ADDRESS <<1, &receivedByte, 1, HAL_MAX_DELAY);
	if (status!=HAL_OK){
			Error_Handler();
		}
		return receivedByte;
 }


 void I2CDelay(uint8_t timeD){
	 HAL_Delay(timeD);
	 	return;
 }
