#ifndef __QUEUES__H
#define __QUEUES__H

#define USART_MSG_LEN   40

#ifdef __cplusplus
extern "C"
{
#endif
#include <zephyr/kernel.h>

extern struct k_msgq shellMsg;
extern struct k_msgq deviceMsg;

#ifdef __cplusplus
}
#endif



#endif