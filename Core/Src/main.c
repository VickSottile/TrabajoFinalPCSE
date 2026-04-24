/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/

#include <delay.h>
#include <stdbool.h>
#include <stm32f4xx_hal_def.h>
#include <stm32f4xx_hal_flash_ex.h>
#include <stm32f4xx_hal_pwr_ex.h>
#include <stm32f4xx_hal_rcc.h>
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/


//UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
#define LED1_DELAY 500
#define LED2_DELAY 100



 delay_t led1;
 tick_t delayV[]={LED1_DELAY, LED2_DELAY};
 bool_t ready=false;
 bool_t running_led1=false;//indica si el led1 ya está running
 tick_t time=LED1_DELAY;;
 ent_t veces=0; //variable para contar las veces que se encendio o apagó el led
 ent_t i=0; //variable auxiliar para recorrer el vector de tiempo
 uint8_t dato;
 bool_t rAvailable;
 led_state_t ledUART = LED_OFF;
 //extern UART_HandleTypeDef huart2; //lo puse extern porque me empezo a causar problemas pero no va asi, mover todo a port.c

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
//static void MX_GPIO_Init(void);
//static void MX_USART2_UART_Init(void);

//static void MX_I2C1_Init(void);
/* USER CODE BEGIN PFP */



/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */





/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
  debounceFSM_init();
  MEF_init();



  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  //MX_USART2_UART_Init();
  //MX_I2C1_Init();


  /* USER CODE BEGIN 2 */
  I2Cinit();
  delayInit(&led1, delayV[i]);
  uartInit();
  //LCD_init();
  //LCD_writeString("Hola Mundillo");
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	 debounceFSM_update();

	 MEFUpdate(); //MEF + UART
	 ledUpdate();

	  /*PUNTO 1 y ECHO*/
	 /* uartReceiveStringSize(&dato, 1);
	  rAvailable = uartDataAvailable(); //almacena true si hay datos aun no leidos
	  if (rAvailable){
	  uartSendStringSize(&dato, 1);
	  }
	  debounceFSM_update();
	  if (readKey())
	  {
		  i++;
		  if (i>(sizeof(delayV)/sizeof(delayV[0]))-1)
		  	{
		  		i=0;
		  	}

				delayWrite(&led1, delayV[i] );

	  }
		  ready = delayRead(&led1);
		  if (ready==true)
		  	 {
		  	     HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
		  	 }
*/
	  //Agregar aca el cambio de frecuencia del parpadeo

		/*ready = delayRead(&led1);
	    if (ready==true)
	    {
	      HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
	      veces++;
	    }
		if (veces>2*BLINK_TIMES)
		{
			//Si ya parpadeó las veces necesarias cambio el tiempo
			veces=0;
			i++;
			if (i>(sizeof(delayV)/sizeof(delayV[0]))-1)
			{
				i=0;
			}
	    running_led1 = delayIsRunning(&led1);
	    if(!running_led1) // si running esta corriendo puede cambiar el valor de delay
			delayWrite(&led1, delayV[i] );
		}



	  }*/


		  }




  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */


/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */


/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */


/* USER CODE BEGIN 4 */



/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */


#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
