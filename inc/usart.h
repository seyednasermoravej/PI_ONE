#ifndef __UART__H
#define __UART__H


#ifdef __cplusplus
extern "C"
{
#endif
#include <zephyr/drivers/uart.h>
#include "main.h"
#include "queues.h"
#include <zephyr/kernel.h>

#define MSG_SIZE    40
#define USART_DEVICE_STACK_SIZE     1024
#define USART_SHELL_STACK_SIZE      1024

#ifdef __cplusplus
}
#endif


#endif