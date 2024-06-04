/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "SEGGER_SYSVIEW.h"
#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>
#include <stdbool.h>

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

/* USER CODE BEGIN PV */

#define DWT_CTRL (*(volatile uint32_t *)0xE0001000)

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
/* USER CODE BEGIN PFP */
void led_red_handler(void *parameters);
void led_orange_handler(void *parameters);
void led_green_handler(void *parameters);

extern void SEGGER_UART_init(uint32_t);

TaskHandle_t green_handle;
TaskHandle_t red_handle;
TaskHandle_t orange_handle;

bool reset = 0;

TaskHandle_t volatile next_task_handle = NULL;

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {
	/* USER CODE BEGIN 1 */

	BaseType_t status;

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
	/* USER CODE BEGIN 2 */

	//Enale the CYCNT counter
	DWT_CTRL |= (1 << 0);

	SEGGER_UART_init(500000);

	SEGGER_SYSVIEW_Conf();

	//SEGGER_SYSVIEW_Start();

	status = xTaskCreate(led_green_handler, "led_green_handler_task", 200,
	NULL, 3, &green_handle);

	configASSERT(status == pdPASS);

	next_task_handle = green_handle;

	status = xTaskCreate(led_red_handler, "led_red_handler_task", 200,
	NULL, 2, &red_handle);

	configASSERT(status == pdPASS);

	status = xTaskCreate(led_orange_handler, "led_orange_handler_task", 200,
	NULL, 1, &orange_handle);

	configASSERT(status == pdPASS);


	//start the freeRTOS scheduler
	vTaskStartScheduler();

	//if the control stop here is not enough FreRTOOS heap.

	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1) {
		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
	}
	/* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };

	/** Configure the main internal regulator output voltage
	 */
	__HAL_RCC_PWR_CLK_ENABLE();
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
	RCC_OscInitStruct.PLL.PLLM = 8;
	RCC_OscInitStruct.PLL.PLLN = 100;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ = 4;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK) {
		Error_Handler();
	}
}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void) {
	GPIO_InitTypeDef GPIO_InitStruct = { 0 };

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOH_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOA, LD2_Pin | Red_Led_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(Orange_Led_GPIO_Port, Orange_Led_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pin : User_blue_Pin */
	GPIO_InitStruct.Pin = User_blue_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(User_blue_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pins : USART_TX_Pin USART_RX_Pin */
	GPIO_InitStruct.Pin = USART_TX_Pin | USART_RX_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/*Configure GPIO pins : LD2_Pin Red_Led_Pin */
	GPIO_InitStruct.Pin = LD2_Pin | Red_Led_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/*Configure GPIO pin : Orange_Led_Pin */
	GPIO_InitStruct.Pin = Orange_Led_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(Orange_Led_GPIO_Port, &GPIO_InitStruct);

	/* EXTI interrupt init*/
	HAL_NVIC_SetPriority(EXTI15_10_IRQn, 6, 0);
	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

}

/* USER CODE BEGIN 4 */

void led_green_handler(void *parameters) {

	BaseType_t status;

	while (1) {
		SEGGER_SYSVIEW_PrintfTarget("Toggling Green");
		HAL_GPIO_TogglePin(GPIOA, LD2_Pin);
		status = xTaskNotifyWait(0, 0, NULL, pdMS_TO_TICKS(1000));
		if (status == pdTRUE) {
			portENTER_CRITICAL();
			next_task_handle = red_handle;
			HAL_GPIO_WritePin(GPIOA, LD2_Pin, GPIO_PIN_SET);
			SEGGER_SYSVIEW_PrintfTarget("Suspend green Task");
			portEXIT_CRITICAL();
			vTaskSuspend(NULL);
		}
	}

}

void led_red_handler(void *parameters) {

	BaseType_t status;

	while (1) {
		SEGGER_SYSVIEW_PrintfTarget("Toggling red");
		HAL_GPIO_TogglePin(GPIOA, Red_Led_Pin);
		status = xTaskNotifyWait(0, 0, NULL, pdMS_TO_TICKS(800));
		if (status == pdTRUE) {
			portENTER_CRITICAL();
			next_task_handle = orange_handle;
			HAL_GPIO_WritePin(GPIOA, Red_Led_Pin, GPIO_PIN_SET);
			SEGGER_SYSVIEW_PrintfTarget("Suspend red Task");
			portEXIT_CRITICAL();
			vTaskSuspend(NULL);
		}
	}

}

void led_orange_handler(void *parameters) {

	BaseType_t status;

	while (1) {
		SEGGER_SYSVIEW_PrintfTarget("Toggling Orange");
		HAL_GPIO_TogglePin(GPIOB, Orange_Led_Pin);
		status = xTaskNotifyWait(0, 0, NULL, pdMS_TO_TICKS(400));
		if (status == pdTRUE) {
			portENTER_CRITICAL();
			reset = 1;
			HAL_GPIO_WritePin(GPIOA, Orange_Led_Pin, GPIO_PIN_SET);
			SEGGER_SYSVIEW_PrintfTarget("Suspend orange Task");
			portEXIT_CRITICAL();
			vTaskSuspend(NULL);
		}

	}
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	if (GPIO_Pin == User_blue_Pin) {
		traceISR_ENTER();
		SEGGER_SYSVIEW_PrintfTarget("Button notification line 13");
		if (!reset) {
			BaseType_t xHigherPriorityTaskWoken;
			xTaskNotifyFromISR(next_task_handle, 0, eNoAction, &xHigherPriorityTaskWoken);
			//to launch the schuduler han will choise the unblock task with the highest priotity
			portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
		} else {
			reset = 0;
			UBaseType_t uxSavedInterruptStatus;
			uxSavedInterruptStatus = taskENTER_CRITICAL_FROM_ISR();
			next_task_handle = green_handle;
			xTaskResumeFromISR(green_handle);
			xTaskResumeFromISR(red_handle);
			xTaskResumeFromISR(orange_handle);
			taskEXIT_CRITICAL_FROM_ISR(uxSavedInterruptStatus);
		}
		traceISR_EXIT();
	}
}

/* USER CODE END 4 */

/**
 * @brief  Period elapsed callback in non blocking mode
 * @note   This function is called  when TIM3 interrupt took place, inside
 * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
 * a global variable "uwTick" used as application time base.
 * @param  htim : TIM handle
 * @retval None
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	/* USER CODE BEGIN Callback 0 */

	/* USER CODE END Callback 0 */
	if (htim->Instance == TIM3) {
		HAL_IncTick();
	}
	/* USER CODE BEGIN Callback 1 */

	/* USER CODE END Callback 1 */
}

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1) {
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
