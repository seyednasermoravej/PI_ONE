#ifndef __CAN__H
#define __CAN__H

#include "main.h"
#include <zephyr/canbus/isotp.h>
#define CONFIG_RX_THREAD_STACK_SIZE      1024
int initCan();

#endif