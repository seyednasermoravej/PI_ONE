#ifndef __PWM__H
#define __PWM__H

#include "main.h"
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/pwm.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/util.h>
#define GREEN_FREQUENCY             100
#define RED_FREQUENCY               200

#define FAN_Reg_speed_IDX           0 
#define DATA_LED_IDX                1 
#define Buzzer_IDX                  2 
#define HRTIM_IDX                   10

#define MAX_PERIOD 10000
void initPwms(float dutycycle);
void pwmSet(uint8_t idx, uint32_t frequency, float dutycycle);
void turnOffAllPWMs();
#endif