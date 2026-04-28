/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * File Name          : freertos.c
 * Description        : Code for freertos applications
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2026 STMicroelectronics.
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
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "FreeRTOS.h"
#include "cmsis_os2.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include"adc.h"
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
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* Hook prototypes */
void configureTimerForRunTimeStats(void);
unsigned long getRunTimeCounterValue(void);

/* USER CODE BEGIN 1 */
/* Functions needed when configGENERATE_RUN_TIME_STATS is on */
__weak void configureTimerForRunTimeStats( void )
{

}

__weak unsigned long getRunTimeCounterValue( void )
{
	return 0;
}
/* USER CODE END 1 */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

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
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
	/* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
	/* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
 * @brief  Function implementing the defaultTask thread.
 * @param  argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
	extern ADC_HandleTypeDef hadc3;
	int32_t filtered = 0;
	/* Infinite loop */
	for( ; ; )
	{
		osDelay( 1000 );
		HAL_ADC_Start_IT( &hadc3 );
		osThreadFlagsWait( 0x0001U , osFlagsWaitAny , osWaitForever );
		uint32_t raw_temp = HAL_ADC_GetValue( &hadc3 );
		HAL_ADC_Start_IT( &hadc3 );
		osThreadFlagsWait( 0x0001U , osFlagsWaitAny , osWaitForever );
		uint32_t raw_vrefint = HAL_ADC_GetValue( &hadc3 );
		HAL_ADC_Stop_IT( &hadc3 );

		// convert to Celsius and print
		uint32_t vdd = VREFINT_CAL_VREF * ( *VREFINT_CAL_ADDR ) / raw_vrefint;
		uint32_t temp_scaled = raw_temp * vdd / VREFINT_CAL_VREF;

		int32_t temp_centi = ( (int32_t) temp_scaled - *TEMPSENSOR_CAL1_ADDR ) * ( ( TEMPSENSOR_CAL2_TEMP - TEMPSENSOR_CAL1_TEMP ) * 100 )
		                / ( *TEMPSENSOR_CAL2_ADDR - *TEMPSENSOR_CAL1_ADDR ) + ( TEMPSENSOR_CAL1_TEMP * 100 );

		// smooth it, the sensor is very noisy
		filtered = ( filtered * 7 + temp_centi ) / 8;

		int32_t integer_part = temp_centi / 100;
		uint32_t decimal_part = temp_centi % 100;
		printf( "Temp = %ld.%02ld C\n" , integer_part , decimal_part );
	}
  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
void HAL_ADC_ConvCpltCallback( ADC_HandleTypeDef* hadc )
{
	osThreadFlagsSet( defaultTaskHandle , 0x0001U );
}
/* USER CODE END Application */

