#ifndef __MAIN__H
#define __MAIN__H
 
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/device.h>
#include <zephyr/drivers/pwm.h>
#include <zephyr/logging/log.h>
#include <stm32g4xx.h>
#include "adcs.h"

#define MASTER

#define RELEASE
#ifdef RELEASE

#elif defined DEBUG
#else
#error "you must define DEBUG or RELEASE"
#endif


#ifdef MASTER 

#elif defined SLAVE 
#else
#error "you must define MASTER or SLAVE."
#endif


#define USART_SHELL_PRIORITY     7
#define USART_DEVICE_PRIORITY    7

#define CONFIG_SAMPLE_RX_THREAD_PRIORITY    7

#define TB_INIT				80
#define VIN_INIT			100
#define VOUT_INIT			100
#define I_IN_INIT			25
#define I_OUT_INIT			25
#define MAX_INPUT_CURRENT	25
#define MAX_OUTPUT_CURRENT	20


void Error_Handler(void);
#endif

