#ifndef __MODBUS__H
#define __MODBUS__H


#include <zephyr/kernel.h>
#include <zephyr/sys/util.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/modbus/modbus.h>

#include <zephyr/logging/log.h>

#include "main.h"
void rtuServerInit();
void rtuClientInit();


#endif