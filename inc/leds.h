#ifndef __LED__H
#define __LED__H

#include "main.h"
#include <zephyr/drivers/gpio.h>
/* The devicetree node identifier for the "led0" alias. */
#define LED0_NODE DT_ALIAS(led0)
int initLeds();

#endif