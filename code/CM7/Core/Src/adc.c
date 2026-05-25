/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    adc.c
 * @brief   This file provides code for the configuration
 *          of the ADC instances.
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
#include "adc.h"

/* USER CODE BEGIN 0 */
#include "tim.h"
#include "lwl.h"

/* Variables ------------------------------------------------------------------*/
static uint16_t adc_buf0[ADC3_NUM_REGS];
static uint16_t adc_buf1[ADC3_NUM_REGS];
static volatile uint16_t* latest_adc_buf = NULL;
/* USER CODE END 0 */

ADC_HandleTypeDef hadc3;
DMA_HandleTypeDef hdma_adc3;

/* ADC3 init function */
void MX_ADC3_Init(void)
{

  /* USER CODE BEGIN ADC3_Init 0 */

  /* USER CODE END ADC3_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC3_Init 1 */

  /* USER CODE END ADC3_Init 1 */

  /** Common config
  */
  hadc3.Instance = ADC3;
  hadc3.Init.Resolution = ADC_RESOLUTION_16B;
  hadc3.Init.ScanConvMode = ADC_SCAN_ENABLE;
  hadc3.Init.EOCSelection = ADC_EOC_SEQ_CONV;
  hadc3.Init.LowPowerAutoWait = DISABLE;
  hadc3.Init.ContinuousConvMode = DISABLE;
  hadc3.Init.NbrOfConversion = 2;
  hadc3.Init.DiscontinuousConvMode = DISABLE;
  hadc3.Init.ExternalTrigConv = ADC_EXTERNALTRIG_T2_TRGO;
  hadc3.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_RISING;
  hadc3.Init.ConversionDataManagement = ADC_CONVERSIONDATA_DMA_CIRCULAR;
  hadc3.Init.Overrun = ADC_OVR_DATA_PRESERVED;
  hadc3.Init.LeftBitShift = ADC_LEFTBITSHIFT_NONE;
  hadc3.Init.OversamplingMode = DISABLE;
  hadc3.Init.Oversampling.Ratio = 8;
  if (HAL_ADC_Init(&hadc3) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_TEMPSENSOR;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_810CYCLES_5;
  sConfig.SingleDiff = ADC_SINGLE_ENDED;
  sConfig.OffsetNumber = ADC_OFFSET_NONE;
  sConfig.Offset = 0;
  sConfig.OffsetSignedSaturation = DISABLE;
  if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_VREFINT;
  sConfig.Rank = ADC_REGULAR_RANK_2;
  if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC3_Init 2 */

  /* USER CODE END ADC3_Init 2 */

}

void HAL_ADC_MspInit(ADC_HandleTypeDef* adcHandle)
{

  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
  if(adcHandle->Instance==ADC3)
  {
  /* USER CODE BEGIN ADC3_MspInit 0 */

  /* USER CODE END ADC3_MspInit 0 */

  /** Initializes the peripherals clock
  */
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_ADC;
    PeriphClkInitStruct.PLL2.PLL2M = 4;
    PeriphClkInitStruct.PLL2.PLL2N = 25;
    PeriphClkInitStruct.PLL2.PLL2P = 4;
    PeriphClkInitStruct.PLL2.PLL2Q = 2;
    PeriphClkInitStruct.PLL2.PLL2R = 2;
    PeriphClkInitStruct.PLL2.PLL2RGE = RCC_PLL2VCIRANGE_3;
    PeriphClkInitStruct.PLL2.PLL2VCOSEL = RCC_PLL2VCOWIDE;
    PeriphClkInitStruct.PLL2.PLL2FRACN = 0;
    PeriphClkInitStruct.AdcClockSelection = RCC_ADCCLKSOURCE_PLL2;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
      Error_Handler();
    }

    /* ADC3 clock enable */
    __HAL_RCC_ADC3_CLK_ENABLE();

    /* ADC3 DMA Init */
    /* ADC3 Init */
    hdma_adc3.Instance = DMA1_Stream0;
    hdma_adc3.Init.Request = DMA_REQUEST_ADC3;
    hdma_adc3.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_adc3.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_adc3.Init.MemInc = DMA_MINC_ENABLE;
    hdma_adc3.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    hdma_adc3.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    hdma_adc3.Init.Mode = DMA_CIRCULAR;
    hdma_adc3.Init.Priority = DMA_PRIORITY_LOW;
    hdma_adc3.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_adc3) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(adcHandle,DMA_Handle,hdma_adc3);

    /* ADC3 interrupt Init */
    HAL_NVIC_SetPriority(ADC3_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(ADC3_IRQn);
  /* USER CODE BEGIN ADC3_MspInit 1 */

  /* USER CODE END ADC3_MspInit 1 */
  }
}

void HAL_ADC_MspDeInit(ADC_HandleTypeDef* adcHandle)
{

  if(adcHandle->Instance==ADC3)
  {
  /* USER CODE BEGIN ADC3_MspDeInit 0 */

  /* USER CODE END ADC3_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_ADC3_CLK_DISABLE();

    /* ADC3 DMA DeInit */
    HAL_DMA_DeInit(adcHandle->DMA_Handle);

    /* ADC3 interrupt Deinit */
    HAL_NVIC_DisableIRQ(ADC3_IRQn);
  /* USER CODE BEGIN ADC3_MspDeInit 1 */

  /* USER CODE END ADC3_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
void DMA_M0_Callback(DMA_HandleTypeDef *hdma)
{
    UNUSED(hdma);

    latest_adc_buf = adc_buf0;
}

void DMA_M1_Callback(DMA_HandleTypeDef *hdma)
{
    UNUSED(hdma);

    latest_adc_buf = adc_buf1;
}

void HAL_ADC_ErrorCallback( ADC_HandleTypeDef* hadc )
{
	UNUSED( hadc );
}

void start_ADC_DMA()
{
    hadc3.DMA_Handle->XferCpltCallback   = DMA_M0_Callback;
    hadc3.DMA_Handle->XferM1CpltCallback = DMA_M1_Callback;

	// Start DMA once
	HAL_DMAEx_MultiBufferStart_IT(
		hadc3.DMA_Handle,
		(uint32_t)&ADC3->DR,
		(uint32_t)adc_buf0,
		(uint32_t)adc_buf1,
		ADC3_NUM_REGS
	);

	HAL_ADC_Start(&hadc3);

	// Start timer
	HAL_TIM_Base_Start( &htim2 );
}

void calc_ADC_temp()
{
	lwl_enter_record( ADC_LWL_ID , ADC_READ_LWL_ID , "" );

	static int32_t filtered = 0;

	// get raw adc measurements and calculate Celsius
	uint16_t* adc_buf = (uint16_t*)latest_adc_buf;

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
	int32_t temp_centi = (int32_t)( ( (int64_t) temp_scaled - *TEMPSENSOR_CAL1_ADDR ) * ( ( TEMPSENSOR_CAL2_TEMP - TEMPSENSOR_CAL1_TEMP ) * 100 )
	        / ( *TEMPSENSOR_CAL2_ADDR - *TEMPSENSOR_CAL1_ADDR ) + ( TEMPSENSOR_CAL1_TEMP * 100 ) );
#endif

	// smooth it, the sensor is very noisy
	filtered = ( filtered * 7 + temp_centi ) / 8;

	// print
	printf( "Temp = %ld.%02lu C\n" , temp_centi / 100 , temp_centi % 100 );
}
/* USER CODE END 1 */

