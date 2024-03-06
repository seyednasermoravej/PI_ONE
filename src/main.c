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

#define DEBUG



LOG_MODULE_REGISTER(main, LOG_LEVEL_DBG);

#if !DT_NODE_EXISTS(DT_NODELABEL(busytft))
#error "Overlay for busytft node not properly defined."
#endif
const struct device *tempSensor = DEVICE_DT_GET(DT_NODELABEL(die_temp));
static const struct gpio_dt_spec busytft =
	GPIO_DT_SPEC_GET_BY_IDX(DT_NODELABEL(busytft), gpios, 0);


void Error_Handler(){}
void initBusyTft()
{
	if(!gpio_is_ready_dt(&busytft))
	{
		LOG_INF("The gpio is not ready.\n");
		return;
	}
	LOG_INF("The gpio is ready to use.\n");
}

void initBoard()
{
	initLeds();

	initBusyTft();

	initAdcs();

	initPwms();
	MX_HRTIM1_Init();
	// initCan();
#ifdef MASTER
	rtuClientInit();
#else
	rtuServerInit();
#endif
}


void automaticMode()
{
	LOG_INF("Entered automatic mode.\n");
}

void COVMode()
{
	uint16_t vIn, iIn, vOut, iOut, vInMax, iInMax, vOutMax, iOutMax, vInMin, iInMin, vOutMin, iOutMin;
	// ovRegulate, ovMaxBattery, ovMinBattery, inMinBattery, inMaxBattery, maxDischargeBatteryCurrent, maxChargeBatteryCurrent;
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
	// LOG_INF("Please enter voltage value to regulate in converter output.\n");
	// #ifdef DEBUG
	// scanf("%d", ovRegulate);
	// #endif
	// LOG_INF("Please enter the minimum and maximum output voltage values for battery protection.\n");
	// #ifdef DEBUG
	// scanf("%d %d", ovMinBattery, ovMaxBattery);
	// #endif
	// LOG_INF("Please enter the minimum and maximum input voltage values for battery protection.\n");
	// #ifdef DEBUG
	// scanf("%d %d", inMinBattery, inMaxBattery);
	// #endif
	// LOG_INF("Please enter max charge and discharge battery current.\n");
	// #ifdef DEBUG
	// scanf("%d %d", maxChargeBatteryCurrent, maxDischargeBatteryCurrent);
	// #endif
}

void COVModeBatChMode()
{
	uint16_t vIn, iIn, vOut, iOut, vInMax, iInMax, vOutMax, iOutMax, vInMin, iInMin, vOutMin, iOutMin;
	// ovRegulate, ovMaxBattery, ovMinBattery, inMinBattery, inMaxBattery, maxDischargeBatteryCurrent, maxChargeBatteryCurrent;
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

}
void uniMode()
{
	LOG_INF("Unidirectional Selected.\n");
	LOG_INF("Please select 1 for COVMode and 2 for COVMode with BatChMode.\n");
#ifdef RELEASE
	uint8_t mode = 0;
	while(k_msgq_get(&lcdMsg, &mode, SYS_FOREVER_MS));
	//reading from lcd.
#else 
	int mode = 0;
	scanf("%d", &mode);
#endif
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
	scanf("%d", &mode);
#else

	uint8_t mode = 0;
	//the mode value must be given from uart lcd.
#endif
	while (!((mode == 1) || (mode == 2)))
	{
		LOG_INF("The entered value is incorrect.\n");
		LOG_INF("Please enter 1 for Unidirectional mode and 2 for Bidirectional mode.\n");
		scanf("%d", &mode);
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
bool initialCheckSequence()
{
#ifdef DEBUG
	printf("What are the values of the following? enter with a sapce between them:\n");
	printf("Now these values are given by the tester user to check the logic of the program.\n");
	printf("In real application these values will be read by the ADC.\n");
	printf("temp temp2 temp_mcu vin vout iIn iOut.\n");
	uint8_t temp, temp2, tempMcu, vIn, vOut, iIn, iOut;
	scanf("%d %d %d %d %d %d %d", &temp, &temp2, &tempMcu, &vIn, &vOut, &iIn, &iOut);
#else
	struct sensor_value tempStruct;
	int32_t temp;
	// uint16_t temp2 = readAdc(TEMP2_IDX);
	// uint16_t tempMcu = readAdc(TEMP_MCU_IDX);
	sensor_sample_fetch(tempSensor);
	uint16_t vIn = readAdc(VIN_IDX);
	uint16_t vOut = readAdc(VOUT_IDX);
	uint16_t iIn = readAdc(I_IN_IDX);
	uint16_t iOut = readAdc(I_OUT_IDX);

	sensor_channel_get(tempSensor, SENSOR_CHAN_DIE_TEMP, &tempStruct);

	LOG_INF("temp: %d.%06d\n",tempStruct.val1, tempStruct.val2);
	temp = tempStruct.val1;

#endif

	// LOG_INF("Temp value is: %d\n", temp);
	// LOG_INF("temp2 value is: %d\n", temp2);
	// LOG_INF("temp mcu value is: %d\n", tempMcu);
	LOG_INF("TB value is: %d\n", temp);
	LOG_INF("vin value is: %d\n", vIn);
	LOG_INF("vout value is: %d\n", vOut);
	LOG_INF("I in value is: %d\n", iIn);
	LOG_INF("I out value is: %d\n", iOut);
	if((temp < TB_INIT) && (vIn < VIN_INIT) && ( vOut < VOUT_INIT) && (iIn < I_IN_INIT) && (iOut < I_OUT_INIT))
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
	while(k_msgq_get(&lcdMsg, &userSelection, SYS_FOREVER_MS));
	LOG_INF("The user has selected option %u.\n", userSelection);
	return userSelection;
}

int main(void)
{

	initBoard();

	bool status = initialCheckSequence();
#ifdef DEBUG
status = true;
#endif

	if(status)
	{
		pwmSet(DATA_LED_IDX, GREEN_FREQUENCY, 50);
		LOG_INF("The led turns green.\n");
	}
	else
	{
		pwmSet(DATA_LED_IDX, RED_FREQUENCY, 50);
		LOG_INF("The led turns red.\n");
		return 1;
	}
	while((status) && (gpio_pin_get_dt(&busytft)));
	int busytft = 1;
	while((status) && (busytft))
	{
		printf("Set the busyness of the LCD, 1 when the lcd is busy and 0 if the lcd is free.\n");
		scanf("%d", &busytft);
	}
	LOG_INF("The LCD is ready to accept commands.\n");
	LOG_INF("1 for automatic mode and 2 for the manual mode.\n");
	int userSelection = tftAccess();
	while(!((userSelection == 1) || (userSelection == 2)))
	{
		LOG_INF("incorrect choice.\n");
		LOG_INF("1 for automatic mode and 2 for the manual mode.\n");
		userSelection = tftAccess();
	}
	if(userSelection == 1)
	{
		automaticMode();
	}
	else
	{
		manualMode();
	}

	bool otherStatuses = true;
	while(1)
	{
		status = initialCheckSequence();
#ifdef MASTER
//receive status of the whole system.
//check status of the whole system.
//if all statuses are ok conitunes
//else send command to all the devices to stop
#else
		//send status to the master
		// receive others status from master
#endif
		if((!status) && (!otherStatuses))
		{
			turnOffAllPWMs();
			pwmSet(DATA_LED_IDX, RED_FREQUENCY, 0.5);
		}	
	}	

	
	return 0;
}