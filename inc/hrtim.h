/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    hrtim.h
  * @brief   This file contains all the function prototypes for
  *          the hrtim.c file
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
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __HRTIM_H__
#define __HRTIM_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "gpio.h"
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

extern HRTIM_HandleTypeDef hhrtim1;

/* USER CODE BEGIN Private defines */
#define MICRO_CLK_170
#ifdef MICRO_CLK_150
/*
  The duration of pwm is equal to (TIMA_PERIOD / (150000000 * 32)).
  So, for the given value, duration = 48000/(150000000 * 32) = 10 us
*/
#define TIMA_PERIOD 48000 
/*
  The dead time between each channel is equal to (DEAD_TIME / (150000000 * 8)).
  For the given value, dead time = 240/(150000000 * 8) = 200 ns.
*/
#define	DEAD_TIME 240	//
#elif defined MICRO_CLK_170

/*
  The duration of pwm is equal to (TIMA_PERIOD / (170000000 * 32)).
  So, for the given value, duration = 54400/(170000000 * 32) = 10 us
*/
#define TIMA_PERIOD 54400 
/*
  The dead time between each channel is equal to (DEAD_TIME / (170000000 * 8)).
  For the given value, dead time = 272/(170000000 * 8) = 200 ns.
*/

#define	DEAD_TIME 272	//
#else
#error "One of MICRO_CLK_150 or MICRO_CLK_170 must be defined"
#endif
/* USER CODE END Private defines */

void MX_HRTIM1_Init(void);

void HAL_HRTIM_MspPostInit(HRTIM_HandleTypeDef *hhrtim);

/* USER CODE BEGIN Prototypes */
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __HRTIM_H__ */

