/*
 * Copyright (c) 2016 Intel Corporation
 * Copyright (c) 2020 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file Sample app to demonstrate PWM.
 */

#include <stm32g4xx_hal.h>
#include <zephyr/drivers/sensor.h>
#include "../inc/main.h"
#include "../inc/leds.h"
#include "../inc/adcs.h"
#include "../inc/pwms.h"
#include "../inc/modbus.h"
#include "hrtim.h"
#include "queues.h"
#include "controllers.h"
#include "math.h"
#include "closedLoop.h"
#include "counters.h"



LOG_MODULE_REGISTER(main, LOG_LEVEL_NONE);

#if !DT_NODE_EXISTS(DT_NODELABEL(busytft))
#error "Overlay for busytft node not properly defined."
#endif
const struct device *tempSensor = DEVICE_DT_GET(DT_NODELABEL(die_temp));
static const struct gpio_dt_spec busytft =
	GPIO_DT_SPEC_GET_BY_IDX(DT_NODELABEL(busytft), gpios, 0);

static const struct gpio_dt_spec stby = GPIO_DT_SPEC_GET_OR(DT_NODELABEL(stby), gpios,
							      {0});
								  
void Error_Handler(){}
bool checkSequence();
void stbyInit();
PIController PI_voltage;


void initBusyTft()
{
	if(!gpio_is_ready_dt(&busytft))
	{
		LOG_INF("The gpio is not ready.\n");
		return;
	}
	LOG_INF("The gpio is ready to use.\n");
}

void stbyInit()
{
    int ret;
	if (!gpio_is_ready_dt(&stby)) {
		printk("Error: button device %s is not ready\n",
		       stby.port->name);
		return 0;
	}
    ret = gpio_pin_configure_dt(&stby, GPIO_OUTPUT_INACTIVE);

	if (ret != 0) {
		printk("Error %d: failed to configure %s pin %d\n",
		       ret, stby.port->name, stby.pin);
		return 0;
	}

}

void initBoard()
{

	initLeds();
	stbyInit();
	// counter();
	initBusyTft();

// MX_ADC1_Init();
	initAdcs();
	
	//Alk : What is the purpose of initPwms(0.5) here ?
	HAL_MspInit();
	initPwms();
	// initCan();
// #ifdef MASTER
// 	rtuClientInit();
// #else
// 	rtuServerInit();
// #endif
}

bool faultHandler(bool status)
{

	if(status)
	{
		pwmSet(DATA_LED_IDX, GREEN_FREQUENCY, 0.5);
		LOG_INF("The led turns green.\n");
	}
	else
	{
		turnOffAllPWMs();
		pwmSet(DATA_LED_IDX, RED_FREQUENCY, 0.5);
		LOG_INF("The led turns red.\n");
		return true;
	}
	return false;
}
void automaticMode()
{
	LOG_INF("Entered automatic mode.\n");
}

void COVMode()
{
	LOG_INF("COVMode is selected.\n");
#ifdef MASTER
	//master calculations for each board
	LOG_INF("Master calculations for each slave.\n");
	//sending parameters for each slave
	LOG_INF("Parameters are sent for each slave.\n");
#else
	//receiving paramters from master.
	LOG_INF("Receiving paramters from master");
#endif
	bool status = true;
	bool fault = false;
	while(count && !fault)// perhaps wait for semaphor to execute
	{
		count = 0;
		status = checkSequence();
		fault = faultHandler(status);
		// voltagePiCotroller();
	}
}

bool getSlaveStatus(uint8_t index)
{
	
}
void COVModeBatChMode()
{
	LOG_INF("COVMode with BatChMode is selected.\n");
#ifdef MASTER
	//master calculations for each board
	LOG_INF("Master calculations for each slave.\n");
	//sending parameters for each slave
	LOG_INF("Parameters are sent for each slave.\n");
#else
	//receiving paramters from master.
	LOG_INF("Receiving paramters from master");
#endif

	bool status = true;
	bool fault = false;
	while(1 && !fault)// perhaps wait for semaphor to execute
	{
		status = checkSequence();
		fault = faultHandler(status);
		// currentPiCotroller();
	}

}
void uniMode()
{
	LOG_INF("Unidirectional Selected.\n");
	LOG_INF("Please select 1 for COVMode and 2 for COVMode with BatChMode.\n");
	uint8_t mode = 0;
	while(k_msgq_get(&lcdMsg, &mode, K_FOREVER));
	//reading from lcd.
	if(mode == 1)
	{
		LOG_INF("COVMode is selected.\n");
		COVMode();
	}
	else if (mode == 2)
	{
		LOG_INF("COVMode with BatChMode is selected.\n");
		COVModeBatChMode();
	}
	else
		LOG_INF("Wrong choice.\n");

}

void powerToOutput()
{
	LOG_INF("Power from input to the output.\n");
}

void powerToInput()
{
	LOG_INF("Power from output to the input.\n");
}

void biMode()
{
	LOG_INF("Entered Bidirectional mode.\n");
	LOG_INF("Please enter voltage battery charge and discharge.\n");
#ifdef DEBUG 
	int vBatCharge, vBatDischarge, inputVoltage;
	printf("Enter voltage baterry charge, discharge and input voltage with space between them.\n");
	scanf("%d %d %d", &vBatCharge, &vBatDischarge, &inputVoltage);
#else
	uint16_t vBatCharge, vBatDischarge, inputVoltage;
	inputVoltage = readAdc(VIN_IDX);
	//vbatCharge and discharge must be provided by user.
#endif
	if(inputVoltage > vBatCharge)
	{
		powerToOutput();
	}
	else
	{
		powerToInput();
	}
}

void manualMode()
{
	LOG_INF("Entered manual mode.\n");
	LOG_INF("Please enter 1 for Unidirectional mode and 2 for Bidirectional mode.\n");
#ifdef DEBUG
	int mode = 0;
	while(k_msgq_get(&lcdMsg, &mode, K_FOREVER));
#else

	float mode = 0;
	//the mode value must be given from uart lcd.
#endif
	while (!((mode == 1) || (mode == 2)))
	{
		LOG_INF("The entered value is incorrect.\n");
		LOG_INF("Please enter 1 for Unidirectional mode and 2 for Bidirectional mode.\n");
		while(k_msgq_get(&lcdMsg, &mode, K_FOREVER));
		// scanf("%d", &mode);
	}	
	if(mode == 1)
	{
		uniMode();
	}
	else
	{
		biMode();
	}
}
bool checkSequence()
{
	uint16_t fan;
#ifdef DEBUG
	printf("What are the values of the following? enter with a sapce between them:\n");
	printf("Now these values are given by the tester user to check the logic of the program.\n");
	printf("In real application these values will be read by the ADC.\n");
	printf("temp temp2 temp_mcu vin vout iIn iOut.\n");
	uint16_t tempSet, temp2Set, tempMcuSet, vInSet, vOutSet, iInSet, iOutSet;
	while(k_msgq_get(&lcdMsg, &tempSet, K_FOREVER));
	while(k_msgq_get(&lcdMsg, &temp2Set, K_FOREVER));
	while(k_msgq_get(&lcdMsg, &tempMcuSet, K_FOREVER));
	while(k_msgq_get(&lcdMsg, &vInSet, K_FOREVER));
	while(k_msgq_get(&lcdMsg, &vOutSet, K_FOREVER));
	while(k_msgq_get(&lcdMsg, &iInSet, K_FOREVER));
	while(k_msgq_get(&lcdMsg, &iOutSet, K_FOREVER));
#elif defined RELEASE
	//struct sensor_value tempStruct;
	//int32_t temp;

	//sensor_sample_fetch(tempSensor);

	//sensor_channel_get(tempSensor, SENSOR_CHAN_DIE_TEMP, &tempStruct);

	//LOG_INF("temp: %d.%06d\n",tempStruct.val1, tempStruct.val2);
	//temp = tempStruct.val1;

#endif
	float temp1 = realTemp(TEMP_IDX);
	LOG_INF("Temp value is: %f\n", temp1);
	float temp2 = realTemp(TEMP2_IDX);
	LOG_INF("Temp2 value is: %f\n", temp2);
	//LOG_DBG("temp mcu value is: %u\n", temp);
	//LOG_DBG("TB value is: %u\n", temp);
	float vIn = realVoltage(VIN_IDX);
	LOG_INF("vin is %f", vIn);
	float vOut = realVoltageN(VOUT_IDX);
	LOG_INF("vout is %f", vOut);
	float iIn = realCurrent(I_IN_IDX);
	LOG_INF("I in value is: %f", iIn);
	float iOut = realCurrent(I_OUT_IDX);
	LOG_INF("I out value is: %f", iOut);


	if((temp1 < TB_INIT) && (vIn < VIN_INIT) && ( vOut < VOUT_INIT) && (iIn < I_IN_INIT) && (iOut < I_OUT_INIT))
	{
		return true;
	}
	else
		return false;
}

int tftAccess()
{
	uint8_t userSelection;
	LOG_DBG("Enter user selection.\n");
	while(k_msgq_get(&lcdMsg, &userSelection, K_FOREVER));
	LOG_INF("The user has selected option %u.\n", userSelection);
	return userSelection;
}

int main(void)
{

	initBoard();
	bool status;
	status = checkSequence();
	pwmSet(HRTIM_IDX, 100000, 0.4);
	//ConfigPIController(&PI_voltage,Kp_i,Ti_i,Up_limit_i,Low_limit_i,F_samp);
	//Ki=Kp/Ti
	gpio_pin_set_dt(&stby, 1);
	ConfigPIController(&PI_voltage,0.17,(0.0017/5),0.7,0.2,170000000);


#ifdef DEBUG
status = true;
#endif
	faultHandler(status);
	int busytft = 0;
#ifdef DEBUG
	printf("Set the busyness of the LCD, 1 when the lcd is busy and 0 if the lcd is free.\n");
	while(k_msgq_get(&lcdMsg, &busytft, K_FOREVER));
	while (status && busytft)
	{
		printf("Set the busyness of the LCD, 1 when the lcd is busy and 0 if the lcd is free.\n");
		while(k_msgq_get(&lcdMsg, &busytft, K_FOREVER));
	}
	
#else
	//while((status) && (gpio_pin_get_dt(&busytft)));
#endif
	LOG_INF("The LCD is ready to accept commands.\n");
	LOG_INF("1 for automatic mode and 2 for the manual mode.\n");
	//int userSelection = tftAccess();
	//while(!((userSelection == 1) || (userSelection == 2)))
	//{
	//	LOG_INF("incorrect choice.\n");
	//	LOG_INF("1 for automatic mode and 2 for the manual mode.\n");
	//	userSelection = tftAccess();
	//}
	//if(userSelection == 1)
	//{
		// automaticMode();
	//}
	//else
	//{
		// manualMode();
	//}

	bool otherStatuses = true;
	while(1)
	{
		// while(count)
		// {


		// status = checkSequence();
#ifdef MASTER
//receive status of the whole system.
//check status of the whole system.
//if all statuses are ok conitunes
//else send command to all the devices to stop
		//for(uint8_t i = 0; i < NUMBERS_OF_SLAVES; i++)
		//{
			// otherStatuses &&= getSlaveStatus(i);
		//}
#else
		//send status to the master
		// receive others status from master
#endif
		// faultHandler(status && otherStatuses);
		//if((!status) && (!otherStatuses))
		//{
		//	pwmSet(DATA_LED_IDX, RED_FREQUENCY, 0.5);
		//}	
		ledTurnOff();
		closedLoop(&PI_voltage, 12000);

		ledTurnOn();
		// int32_t val = realVoltage(2);
		count = 0;
		// }
	}	
	return 0;
}
