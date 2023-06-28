/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdbool.h>
#include "stdio.h"
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
RTC_HandleTypeDef hrtc;

PCD_HandleTypeDef hpcd_USB_FS;

osThreadId NormalStageHandle;
osThreadId MonEDSMEnHandle;
osThreadId MonHResetReqHandle;
osThreadId MonSResetReqHandle;
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_RTC_Init(void);
static void MX_USB_PCD_Init(void);
void NormalStage_init(void const * argument);
void MonEDSMEn_init(void const * argument);
void MonHResetReq_init(void const * argument);
void MonSResetReq_init(void const * argument);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

int _write(int file, char *ptr, int len)
{
	int i=0;
	for(i=0; i<len; i++)
		ITM_SendChar((*ptr++));
	return len;
}

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

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_RTC_Init();
  MX_USB_PCD_Init();
  /* USER CODE BEGIN 2 */

  printf("Starting...\n");

  /* USER CODE END 2 */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of NormalStage */
  osThreadDef(NormalStage, NormalStage_init, osPriorityLow, 0, 128);
  NormalStageHandle = osThreadCreate(osThread(NormalStage), NULL);

  /* definition and creation of MonEDSMEn */
  osThreadDef(MonEDSMEn, MonEDSMEn_init, osPriorityAboveNormal, 0, 128);
  MonEDSMEnHandle = osThreadCreate(osThread(MonEDSMEn), NULL);

  /* definition and creation of MonHResetReq */
  osThreadDef(MonHResetReq, MonHResetReq_init, osPriorityNormal, 0, 128);
  MonHResetReqHandle = osThreadCreate(osThread(MonHResetReq), NULL);

  /* definition and creation of MonSResetReq */
  osThreadDef(MonSResetReq, MonSResetReq_init, osPriorityNormal, 0, 128);
  MonSResetReqHandle = osThreadCreate(osThread(MonSResetReq), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* Start scheduler */
  osKernelStart();
  /* We should never get here as control is now taken by the scheduler */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;
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
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB|RCC_PERIPHCLK_RTC;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
  PeriphClkInit.USBClockSelection = RCC_USBCLKSOURCE_PLL_DIV1_5;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */

  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */

  /* USER CODE END RTC_Init 2 */

}

/**
  * @brief USB Initialization Function
  * @param None
  * @retval None
  */
static void MX_USB_PCD_Init(void)
{

  /* USER CODE BEGIN USB_Init 0 */

  /* USER CODE END USB_Init 0 */

  /* USER CODE BEGIN USB_Init 1 */

  /* USER CODE END USB_Init 1 */
  hpcd_USB_FS.Instance = USB;
  hpcd_USB_FS.Init.dev_endpoints = 8;
  hpcd_USB_FS.Init.speed = PCD_SPEED_FULL;
  hpcd_USB_FS.Init.phy_itface = PCD_PHY_EMBEDDED;
  hpcd_USB_FS.Init.low_power_enable = DISABLE;
  hpcd_USB_FS.Init.lpm_enable = DISABLE;
  hpcd_USB_FS.Init.battery_charging_enable = DISABLE;
  if (HAL_PCD_Init(&hpcd_USB_FS) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USB_Init 2 */

  /* USER CODE END USB_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOF, H_RESET_Pin|S_RESET_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(TEST_EN_GPIO_Port, TEST_EN_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, LD1_Pin|LD3_Pin|LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(USB_PowerSwitchOn_GPIO_Port, USB_PowerSwitchOn_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, EN_RAIL_0_Pin|EN_RAIL_1_Pin|EN_RAIL_2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : H_RESET_REQ_Pin S_RESET_REQ_Pin */
  GPIO_InitStruct.Pin = H_RESET_REQ_Pin|S_RESET_REQ_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pin : USER_Btn_Pin */
  GPIO_InitStruct.Pin = USER_Btn_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(USER_Btn_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PGOOD_RAIL_0_Pin PGOOD_RAIL_2_Pin */
  GPIO_InitStruct.Pin = PGOOD_RAIL_0_Pin|PGOOD_RAIL_2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

  /*Configure GPIO pin : PGOOD_RAIL_1_Pin */
  GPIO_InitStruct.Pin = PGOOD_RAIL_1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(PGOOD_RAIL_1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : H_RESET_Pin S_RESET_Pin TEST_EN_Pin */
  GPIO_InitStruct.Pin = H_RESET_Pin|S_RESET_Pin|TEST_EN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

  /*Configure GPIO pins : LD1_Pin LD3_Pin LD2_Pin */
  GPIO_InitStruct.Pin = LD1_Pin|LD3_Pin|LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : EDSM_EN_Pin */
  GPIO_InitStruct.Pin = EDSM_EN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(EDSM_EN_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : USB_PowerSwitchOn_Pin */
  GPIO_InitStruct.Pin = USB_PowerSwitchOn_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(USB_PowerSwitchOn_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : USB_OverCurrent_Pin */
  GPIO_InitStruct.Pin = USB_OverCurrent_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(USB_OverCurrent_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : EN_RAIL_0_Pin EN_RAIL_1_Pin EN_RAIL_2_Pin */
  GPIO_InitStruct.Pin = EN_RAIL_0_Pin|EN_RAIL_1_Pin|EN_RAIL_2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/* USER CODE BEGIN Header_NormalStage_init */
/**
  * @brief  Function implementing the NormalStage thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_NormalStage_init */
void NormalStage_init(void const * argument)
{
  /* USER CODE BEGIN 5 */

	/**********************************************************************************************
	*
	* This task/thread handles the power sequencing on start up as well as the reverse sequencing if a rail
	* is powered in an incorrect order. This is performed by reading the status of the GPIO pin of
	* each rail to see the next appropriate step.
	*
	* TO ADD/FIX:
	* - Incorporate ADC monitoring if condition that all PGOOD are true/on
	* - Not robust as every condition must be defined, more rails will be more difficult to implement,
	* therefore should implement more portable/flexible statements
	* - Timing requirements to be added in conditions
	*
	**********************************************************************************************/

  /* Infinite loop */
	for(;;)
	{
		// If all rails are off, enable rail 0
		if ((HAL_GPIO_ReadPin(PGOOD_RAIL_0_GPIO_Port,PGOOD_RAIL_0_Pin) == false) &&
				(HAL_GPIO_ReadPin(PGOOD_RAIL_1_GPIO_Port,PGOOD_RAIL_1_Pin) == false) &&
				(HAL_GPIO_ReadPin(PGOOD_RAIL_2_GPIO_Port,PGOOD_RAIL_2_Pin) == false))
		{
			HAL_GPIO_WritePin(EN_RAIL_0_GPIO_Port, EN_RAIL_0_Pin, SET);
		}
		// If all rail 0 is on and rails 1 and 2 are off, enable rail 1
		else if((HAL_GPIO_ReadPin(PGOOD_RAIL_0_GPIO_Port,PGOOD_RAIL_0_Pin) == true) &&
				(HAL_GPIO_ReadPin(PGOOD_RAIL_1_GPIO_Port,PGOOD_RAIL_1_Pin) == false) &&
				(HAL_GPIO_ReadPin(PGOOD_RAIL_2_GPIO_Port,PGOOD_RAIL_2_Pin) == false))
		{
			HAL_GPIO_WritePin(EN_RAIL_1_GPIO_Port, EN_RAIL_1_Pin, SET);
		}
		// If rail 0 and 1 are on and rail 2 is off, enable rail 2
		else if ((HAL_GPIO_ReadPin(PGOOD_RAIL_0_GPIO_Port,PGOOD_RAIL_0_Pin) == true) &&
				(HAL_GPIO_ReadPin(PGOOD_RAIL_1_GPIO_Port,PGOOD_RAIL_1_Pin) == true) &&
				(HAL_GPIO_ReadPin(PGOOD_RAIL_2_GPIO_Port,PGOOD_RAIL_2_Pin) == false))
		{
			HAL_GPIO_WritePin(EN_RAIL_2_GPIO_Port, EN_RAIL_2_Pin, SET);
		}
		// If all rails are on, start voltage monitoring
		else if ((HAL_GPIO_ReadPin(PGOOD_RAIL_0_GPIO_Port,PGOOD_RAIL_0_Pin) == true) &&
				(HAL_GPIO_ReadPin(PGOOD_RAIL_1_GPIO_Port,PGOOD_RAIL_1_Pin) == true) &&
				(HAL_GPIO_ReadPin(PGOOD_RAIL_2_GPIO_Port,PGOOD_RAIL_2_Pin) == true))
		{
			// Implement ADC voltage monitoring function here
			osDelay(1);
		}
		// If any of the rails are off while there are rails after it on, reverse power sequence
		else
		{
			HAL_GPIO_WritePin(EN_RAIL_2_GPIO_Port, EN_RAIL_2_Pin, RESET);
			HAL_GPIO_WritePin(EN_RAIL_1_GPIO_Port, EN_RAIL_1_Pin, RESET);
			HAL_GPIO_WritePin(EN_RAIL_0_GPIO_Port, EN_RAIL_0_Pin, RESET);

		}
		osDelay(1);
	}
  /* USER CODE END 5 */
}

/* USER CODE BEGIN Header_MonEDSMEn_init */
/**
* @brief Function implementing the MonEDSMEn thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_MonEDSMEn_init */
void MonEDSMEn_init(void const * argument)
{
  /* USER CODE BEGIN MonEDSMEn_init */

	/**********************************************************************************************
	*
	* This task/thread is polls for an EDSM enable signal. As of now, a counter edsm is used to see
	* if the truly polls if the GPIO pin reads a high signal. A blink of an LED will signal it has
	* entered EDSM
	*
	* TO ADD/FIX:
	* - Potentially add EDSM function here
	*
	**********************************************************************************************/

	uint16_t edsm = 0;
	printf("Entered MonEDSMEn_init\n");
  /* Infinite loop */
  for(;;)
  {
	if (HAL_GPIO_ReadPin(EDSM_EN_GPIO_Port,EDSM_EN_Pin) == true)
	{
		HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, SET);
		osDelay(250);
		HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, RESET);
		edsm++;
	}
	osDelay(1);
  }
  /* USER CODE END MonEDSMEn_init */
}

/* USER CODE BEGIN Header_MonHResetReq_init */
/**
* @brief Function implementing the MonHResetReq thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_MonHResetReq_init */
void MonHResetReq_init(void const * argument)
{
  /* USER CODE BEGIN MonHResetReq_init */

	/**********************************************************************************************
	*
	* This task/thread is polls for a hard reset request. Upon receiving a hard reset request, it
	* will send a hard reset signal on an output pin for .500 seconds
	*
	* TO ADD/FIX:
	* -
	*
	**********************************************************************************************/

	printf("Entered MonHResetReq_init");

  /* Infinite loop */
  for(;;)
  {
	if (HAL_GPIO_ReadPin(H_RESET_REQ_GPIO_Port,H_RESET_REQ_Pin) == true)
	{
		HAL_GPIO_WritePin(H_RESET_GPIO_Port, H_RESET_Pin, SET);
		osDelay(500);
		HAL_GPIO_WritePin(H_RESET_GPIO_Port, H_RESET_Pin, RESET);
	}
	osDelay(1);
  }
  /* USER CODE END MonHResetReq_init */
}

/* USER CODE BEGIN Header_MonSResetReq_init */
/**
* @brief Function implementing the MonSResetReq thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_MonSResetReq_init */
void MonSResetReq_init(void const * argument)
{
  /* USER CODE BEGIN MonSResetReq_init */

	/**********************************************************************************************
	*
	* This task/thread is polls for a soft reset request. Upon receiving a soft reset request, it
	* will send a hard reset signal on an output pin for .500 seconds
	*
	* TO ADD/FIX:
	* -
	*
	**********************************************************************************************/

	printf("Entered MonSResetReq_init");

  /* Infinite loop */
  for(;;)
  {
	if (HAL_GPIO_ReadPin(S_RESET_REQ_GPIO_Port,S_RESET_REQ_Pin) == true)
	{
		HAL_GPIO_WritePin(S_RESET_GPIO_Port, S_RESET_Pin, SET);
		osDelay(500);
		HAL_GPIO_WritePin(S_RESET_GPIO_Port, S_RESET_Pin, RESET);
	}
	osDelay(1);
  }
  /* USER CODE END MonSResetReq_init */
}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM6 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM6) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

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