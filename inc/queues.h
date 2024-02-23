#ifndef __QUEUES__H
#define __QUEUES__H

#define USART_MSG_LEN   40

#ifdef __cplusplus
extern "C"
{
#endif
#include "main.h"
#ifdef MICRO
#include <zephyr/kernel.h>

extern struct k_msgq deviceMsg;
#endif
#ifdef __cplusplus
}
#endif



#endif