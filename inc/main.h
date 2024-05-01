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

// #define CONFIG_SAMPLE_RX_THREAD_PRIORITY    7


// shall change TB_INIT to TB_INIT_MAX?
#define TB_INIT				100
#define TB_INIT_MIN			0
#define VIN_INIT			50000//50 V limit
#define VIN_INIT_MIN		0
#define VOUT_INIT			50000
#define VOUT_INIT_MIN       0
#define I_IN_INIT			2500000 // high value 
#define I_OUT_INIT			2500000 // high value to no trigger protection 

//#define MAX_INPUT_CURRENT	25
//#define MAX_OUTPUT_CURRENT 20
// INIT MEAN LIMITATION FOR BOARD PROTECTION 
//MAX TEMP - MIN TEMP - MAX VOUT - MIN VOUT - MAX VIN - MIN VIN - MAX IN -  MIN IN 
//ALL TIME CHECKUP IS TO MAKE SURE WE ARE IN THE INTERVAL DEFINE BY THESES VALUES 

#define NUMBERS_OF_SLAVES   3

#define VREF                2500
void Error_Handler(void);

void HAL_MspInit(void);
#endif

