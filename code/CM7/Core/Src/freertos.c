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
#include "adc.h"
#include "tim.h"
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
const osThreadAttr_t defaultTask_attributes =
        { .name = "defaultTask" , .stack_size = 128 * 4 , .priority = (osPriority_t) osPriorityNormal , };

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask( void* argument );

void MX_FREERTOS_Init( void ); /* (MISRA C 2004 rule 8.1) */

/* Hook prototypes */
void configureTimerForRunTimeStats( void );
unsigned long getRunTimeCounterValue( void );

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
void MX_FREERTOS_Init( void )
{
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
	defaultTaskHandle = osThreadNew( StartDefaultTask , NULL , &defaultTask_attributes );

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
void StartDefaultTask( void* argument )
{
	/* USER CODE BEGIN StartDefaultTask */
	UNUSED( argument );

	uint16_t adc_buf[ADC3_NUM_REGS];

	// Start DMA once
	HAL_ADC_Start_DMA( &hadc3 , (uint32_t*) adc_buf , ADC3_NUM_REGS );
	__HAL_DMA_DISABLE_IT( hadc3.DMA_Handle , DMA_IT_HT ); // Don't need half transfer interrupts

	// Start timer
	HAL_TIM_Base_Start( &htim2 );

	int32_t filtered = 0;
	for( ; ; )
	{
		osThreadFlagsWait( 0x0001U , osFlagsWaitAny , osWaitForever );

		// get raw adc measurements and calculate Celsius
#define AVOID_DIVISION 0
#if AVOID_DIVISION == 1
		// calculate
		const uint32_t TEMP_NUM = ( TEMPSENSOR_CAL2_TEMP - TEMPSENSOR_CAL1_TEMP ) * 100;
		const uint32_t TEMP_OFF = TEMPSENSOR_CAL1_TEMP * 100;
		// use q2.30 fixed point reciprocate to avoid the division
		const uint32_t TEMP_RECIP_Q30 = (uint32_t) ( ( (uint64_t) 1 << 30 ) / ( *TEMPSENSOR_CAL2_ADDR - *TEMPSENSOR_CAL1_ADDR ) );

		uint32_t temp = (uint32_t) ( ( (uint64_t) adc_buf[TEMP_REG] * ( *VREFINT_CAL_ADDR ) ) / adc_buf[VDD_REG] );

		int32_t temp_centi = (int32_t) ( ( (int64_t) ( temp - ( *TEMPSENSOR_CAL1_ADDR ) ) * TEMP_NUM * TEMP_RECIP_Q30 ) >> 30 ) + TEMP_OFF;

#else
		uint32_t vdd = VREFINT_CAL_VREF * ( *VREFINT_CAL_ADDR ) / adc_buf[VDD_REG];
		uint32_t temp_scaled = adc_buf[TEMP_REG] * vdd / VREFINT_CAL_VREF;
		int32_t temp_centi = ( (int64_t) temp_scaled - *TEMPSENSOR_CAL1_ADDR ) * ( ( TEMPSENSOR_CAL2_TEMP - TEMPSENSOR_CAL1_TEMP ) * 100 )
		                / ( *TEMPSENSOR_CAL2_ADDR - *TEMPSENSOR_CAL1_ADDR ) + ( TEMPSENSOR_CAL1_TEMP * 100 );
#endif

		// smooth it, the sensor is very noisy
		filtered = ( filtered * 7 + temp_centi ) / 8;

		// print
		printf( "Temp = %ld.%02lu C\n" , temp_centi / 100 , temp_centi % 100 );
	}
	/* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
void HAL_ADC_ConvCpltCallback( ADC_HandleTypeDef* hadc )
{
//	printf("HAL_ADC_ConvCpltCallback\n");
	UNUSED( hadc );
	osThreadFlagsSet( defaultTaskHandle , 0x0001U );
}

void HAL_ADC_ConvHalfCpltCallback( ADC_HandleTypeDef* hadc )
{
//	printf("HAL_ADC_ConvHalfCpltCallback\n");
	UNUSED( hadc );
}

void HAL_ADC_ErrorCallback( ADC_HandleTypeDef* hadc )
{
	UNUSED( hadc );
}
/* USER CODE END Application */

