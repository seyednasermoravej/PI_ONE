#ifndef __MAIN__H
#define __MAIN__H



#define MICRO 
#ifdef PC
#define LOG_INF	printf
#define printk  printf
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#elif defined MICRO
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/device.h>
#include <zephyr/drivers/pwm.h>
#include <zephyr/logging/log.h>
#include "main.h"
#include "adcs.h"


#define USART_SHELL_PRIORITY     7
#define USART_DEVICE_PRIORITY    7

#else 
#error "You must define MICRO or PC."

#endif

#define TB_INIT				80
#define VIN_INIT			100
#define VOUT_INIT			100
#define I_IN_INIT			25
#define I_OUT_INIT			25
#define MAX_INPUT_CURRENT	25
#define MAX_OUTPUT_CURRENT	20

#define MASTER 

#ifdef MASTER

#elif defined SLAVE

#else 
#error "You must choose from MASTER and SLAVE."

#endif
#endif

