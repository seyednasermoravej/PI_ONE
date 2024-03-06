#ifndef __ADC__H
#define __ADC__H

#define TEMP_IDX                    0
#define TEMP2_IDX                   1
#define VIN_IDX                     2 
#define I_IN_IDX                    3 
#define VOUT_IDX                    4 
#define I_OUT_IDX                   5 
#define TEMP_MCU_IDX                6
#define FAN_IN_IDX                  7 

#include "main.h"

/*adc*/
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/adc.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/util.h>


int initAdcs();
uint16_t readAdc(uint8_t index);

#endif