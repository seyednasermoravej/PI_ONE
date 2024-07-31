#include "../inc/adcs.h"


#if !DT_NODE_EXISTS(DT_PATH(zephyr_user)) || \
	!DT_NODE_HAS_PROP(DT_PATH(zephyr_user), io_channels)
#error "No suitable devicetree overlay specified"
#endif

#define ADC_DT_SPEC_AND_COMMA(node_id, prop, idx) \
	ADC_DT_SPEC_GET_BY_IDX(node_id, idx),

/* Data of ADC io-channels specified in devicetree. */
static const struct adc_dt_spec adc_channels[] = {
	DT_FOREACH_PROP_ELEM(DT_PATH(zephyr_user), io_channels,
			     ADC_DT_SPEC_AND_COMMA)
};
LOG_MODULE_REGISTER(adcs, LOG_LEVEL_DBG);

int initAdcs()
{
	/* Configure channels individually prior to sampling. */
	for (size_t i = 0; i < TEMP_MCU_IDX; i++) {
		if(i != 2)
		{
		if (!adc_is_ready_dt(&adc_channels[i])) {
			printk("ADC controller device %s not ready\n", adc_channels[i].dev->name);
			return 0;
		}

	volatile int er = adc_channel_setup_dt(&adc_channels[i]);
		if (er < 0) {
			printk("Could not setup channel #%d (%d)\n", i, er);
			return 0;
		}

		}
	}

	LOG_INF("ADC initialization finished.\n");
}


static int32_t readAdc(uint8_t index)
{
	uint16_t buf;
	struct adc_sequence sequence = {
		.buffer = &buf,
		/* buffer size in bytes, not number of samples */
		.buffer_size = sizeof(buf),
		.calibrate = true,
	};

	int32_t val_mv;

	// LOG_INF("- %s, channel %d: ",
	// 		adc_channels[index].dev->name,
	// 		adc_channels[index].channel_id);

	(void)adc_sequence_init_dt(&adc_channels[index], &sequence);

	int err = adc_read_dt(&adc_channels[index], &sequence);
	// if (err < 0) {
	// 	LOG_INF("Could not read (%d)\n", err);
	// }

	val_mv = (int32_t)buf;

	// LOG_INF("%"PRId32, val_mv);
	err = adc_raw_to_millivolts_dt(&adc_channels[index],
						&val_mv);
	/* conversion to mV may not be supported, skip if not */
		// if (err < 0) {
		// 	printk(" (value in mV not available)\n");
		// } else {
		// 	printk(" = %"PRId32" mV\n", val_mv);
		// }
	return val_mv;
    
}



float realTemp(uint8_t index)
{
	float R , T ;
	float T0 = (273 + 25);
	float R0 = 10000;
	float B = 3450;
	int32_t raw = readAdc(index);
	for(int i=0;i<10000;i++)
	{
		raw = raw + readAdc(index);
	}
	raw=raw/10000;
	R = ( (raw*20000)/(3300 - raw) ); 
	T =  B/( log(R) - log(R0) + (B/T0));
	return (T - 273);
		
}



float realCurrent(uint8_t index)
{
	
	int32_t raw=0 ;
	int32_t offst=450 ;
	
	for(int i=0;i<10000;i++)
	{
		raw = raw + readAdc(index);
	}
	
	raw=(raw/10000)-offst;
	
	float real = (raw - 1000) / 50;
	

	return real;
}


float realVoltage(uint8_t index)
{
 	int32_t raw=0 ;
	int32_t offst=350 ;

	for(int i=0;i<10000;i++)
	{
		raw = raw + readAdc(index);
	}
	
	raw=(raw/10000)-offst;
	
	float real = (raw - 1019) * 89.7606;

	return real;
}


float fanSpeed()
{
	int32_t fan;
	fan = readAdc(FAN_IN_IDX);
	LOG_INF("Fan value is: %u.", fan);
}






/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    adc.c
  * @brief   This file provides code for the configuration
  *          of the ADC instances.
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


/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

// ADC_HandleTypeDef hadc1;

// /* ADC1 init function */
// void MX_ADC1_Init(void)
// {

//   /* USER CODE BEGIN ADC1_Init 0 */

//   /* USER CODE END ADC1_Init 0 */

//   ADC_MultiModeTypeDef multimode = {0};
//   ADC_ChannelConfTypeDef sConfig = {0};

//   /* USER CODE BEGIN ADC1_Init 1 */

//   /* USER CODE END ADC1_Init 1 */

//   /** Common config
//   */
//   hadc1.Instance = ADC1;
//   hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
//   hadc1.Init.Resolution = ADC_RESOLUTION_12B;
//   hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
//   hadc1.Init.GainCompensation = 0;
//   hadc1.Init.ScanConvMode = ADC_SCAN_ENABLE;
//   hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
//   hadc1.Init.LowPowerAutoWait = DISABLE;
//   hadc1.Init.ContinuousConvMode = ENABLE;
//   hadc1.Init.NbrOfConversion = 2;
//   hadc1.Init.DiscontinuousConvMode = DISABLE;
//   hadc1.Init.ExternalTrigConv = ADC_EXTERNALTRIG_HRTIM_TRG1;
//   hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_RISING;
//   hadc1.Init.DMAContinuousRequests = DISABLE;
//   hadc1.Init.Overrun = ADC_OVR_DATA_OVERWRITTEN;
//   hadc1.Init.OversamplingMode = DISABLE;
//   if (HAL_ADC_Init(&hadc1) != HAL_OK)
//   {
//     Error_Handler();
//   }

//   /** Configure the ADC multi-mode
//   */
//   multimode.Mode = ADC_MODE_INDEPENDENT;
//   if (HAL_ADCEx_MultiModeConfigChannel(&hadc1, &multimode) != HAL_OK)
//   {
//     Error_Handler();
//   }

//   /** Configure Regular Channel
//   */
//   sConfig.Channel = ADC_CHANNEL_9;
//   sConfig.Rank = ADC_REGULAR_RANK_1;
//   sConfig.SamplingTime = ADC_SAMPLETIME_2CYCLES_5;
//   sConfig.SingleDiff = ADC_SINGLE_ENDED;
//   sConfig.OffsetNumber = ADC_OFFSET_NONE;
//   sConfig.Offset = 0;
//   if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
//   {
//     Error_Handler();
//   }

//   /** Configure Regular Channel
//   */
//   sConfig.Rank = ADC_REGULAR_RANK_2;
//   if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
//   {
//     Error_Handler();
//   }
//   /* USER CODE BEGIN ADC1_Init 2 */

//   /* USER CODE END ADC1_Init 2 */

// }

// void HAL_ADC_MspInit(ADC_HandleTypeDef* adcHandle)
// {

//   GPIO_InitTypeDef GPIO_InitStruct = {0};
//   RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};
//   if(adcHandle->Instance==ADC1)
//   {
//   /* USER CODE BEGIN ADC1_MspInit 0 */

//   /* USER CODE END ADC1_MspInit 0 */

//   /** Initializes the peripherals clocks
//   */
//     PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC12;
//     PeriphClkInit.Adc12ClockSelection = RCC_ADC12CLKSOURCE_SYSCLK;
//     if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
//     {
//       Error_Handler();
//     }

//     /* ADC1 clock enable */
//     __HAL_RCC_ADC12_CLK_ENABLE();

//     __HAL_RCC_GPIOC_CLK_ENABLE();
//     /**ADC1 GPIO Configuration
//     PA1     ------> ADC1_IN2
//     */
//     GPIO_InitStruct.Pin = GPIO_PIN_3;
//     GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
//     GPIO_InitStruct.Pull = GPIO_NOPULL;
//     HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

//   /* USER CODE BEGIN ADC1_MspInit 1 */

//   /* USER CODE END ADC1_MspInit 1 */
//   }
// }

// void HAL_ADC_MspDeInit(ADC_HandleTypeDef* adcHandle)
// {

//   if(adcHandle->Instance==ADC1)
//   {
//   /* USER CODE BEGIN ADC1_MspDeInit 0 */

//   /* USER CODE END ADC1_MspDeInit 0 */
//     /* Peripheral clock disable */
//     __HAL_RCC_ADC12_CLK_DISABLE();

//     /**ADC1 GPIO Configuration
//     PA1     ------> ADC1_IN2
//     */
//     HAL_GPIO_DeInit(GPIOC, GPIO_PIN_3);

//   /* USER CODE BEGIN ADC1_MspDeInit 1 */

//   /* USER CODE END ADC1_MspDeInit 1 */
//   }
// }

// /* USER CODE BEGIN 1 */

// /* USER CODE END 1 */