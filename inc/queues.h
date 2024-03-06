#ifndef __QUEUES__H
#define __QUEUES__H

#define USART_MSG_LEN   40

#ifdef __cplusplus
extern "C"
{
#endif
#include "main.h"
#include <zephyr/kernel.h>

extern struct k_msgq lcdMsg;
#ifdef __cplusplus
}
#endif



#endif