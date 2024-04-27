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




LOG_MODULE_REGISTER(main, LOG_LEVEL_DBG);

#if !DT_NODE_EXISTS(DT_NODELABEL(busytft))
#error "Overlay for busytft node not properly defined."
#endif
const struct device *tempSensor = DEVICE_DT_GET(DT_NODELABEL(die_temp));
static const struct gpio_dt_spec busytft =
	GPIO_DT_SPEC_GET_BY_IDX(DT_NODELABEL(busytft), gpios, 0);


void Error_Handler(){}
float rawCurrentToRealCurrent(uint8_t index);
float rawVoltageToRealVoltage(uint8_t index);
float rawTempToRealTemp(uint8_t index);

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
	//initPwms(0.5);
	//Alk : What is the purpose of initPwms(0.5) here ?
	HAL_MspInit();

	usartInit();
	// initCan();
// #ifdef MASTER
// 	rtuClientInit();
// #else
// 	rtuServerInit();
// #endif
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

bool getSlaveStatus(uint8_t index)
{
	
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
	lcdMsg()("%d", &mode);
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
bool initialCheckSequence()
{
#ifdef DEBUG
	printf("What are the values of the following? enter with a sapce between them:\n");
	printf("Now these values are given by the tester user to check the logic of the program.\n");
	printf("In real application these values will be read by the ADC.\n");
	printf("temp temp2 temp_mcu vin vout iIn iOut.\n");
	uint16_t temp, temp2, tempMcu, vIn, vOut, iIn, iOut;
	while(k_msgq_get(&lcdMsg, &temp, K_FOREVER));
	while(k_msgq_get(&lcdMsg, &temp2, K_FOREVER));
	while(k_msgq_get(&lcdMsg, &tempMcu, K_FOREVER));
	while(k_msgq_get(&lcdMsg, &vIn, K_FOREVER));
	while(k_msgq_get(&lcdMsg, &vOut, K_FOREVER));
	while(k_msgq_get(&lcdMsg, &iIn, K_FOREVER));
	while(k_msgq_get(&lcdMsg, &iOut, K_FOREVER));
#elif defined RELEASE
	//struct sensor_value tempStruct;
	//int32_t temp;

	//sensor_sample_fetch(tempSensor);
	uint16_t vInRaw = readAdc(VIN_IDX);
	uint16_t vOutRaw = readAdc(VOUT_IDX);
	uint16_t iInRaw = readAdc(I_IN_IDX);
	uint16_t iOutRaw = readAdc(I_OUT_IDX);
	uint16_t fan = readAdc(FAN_IN_IDX);

	//sensor_channel_get(tempSensor, SENSOR_CHAN_DIE_TEMP, &tempStruct);

	//LOG_INF("temp: %d.%06d\n",tempStruct.val1, tempStruct.val2);
	//temp = tempStruct.val1;

#endif
	float temp1=rawTempToRealTemp(TEMP_IDX);
	LOG_DBG("Temp value is: %u\n", temp1);
	float temp2=rawTempToRealTemp(TEMP2_IDX);
	LOG_DBG("temp2 value is: %u\n", temp2);
	//LOG_DBG("temp mcu value is: %u\n", temp);
	//LOG_DBG("TB value is: %u\n", temp);
	LOG_DBG("vin raw value is: %u\n", vInRaw);
	float vIn = rawVoltageToRealVoltage(VIN_IDX);
	LOG_INF("vin is %f", vIn);
	LOG_DBG("vout raw value is: %u\n", vOutRaw);
	float vOut = rawVoltageToRealVoltage(VOUT_IDX);
	LOG_INF("vout is %f", vOut);
	LOG_DBG("I in raw value is: %u\n", iInRaw);
	float iIn = rawCurrentToRealCurrent(I_IN_IDX);
	LOG_INF("I in value is: %f", iIn);
	LOG_DBG("I out raw value is: %u\n", iOutRaw);
	float iOut = rawCurrentToRealCurrent(I_OUT_IDX);
	LOG_INF("I out value is: %f", iOut);
	LOG_DBG("fan in value is: %u\n", fan);
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
	//pwmSet(HRTIM_IDX, 100000, 0.3);
	//turnOffAllPWMs();
	bool status;
	while(1)
	{
		status = initialCheckSequence();
		ledToggle();
		k_msleep(1000);
	}
#ifdef DEBUG
status = true;
#endif

	if(status)
	{
		pwmSet(DATA_LED_IDX, GREEN_FREQUENCY, 0.5);
		LOG_INF("The led turns green.\n");
	}
	else
	{
		pwmSet(DATA_LED_IDX, RED_FREQUENCY, 0.5);
		LOG_INF("The led turns red.\n");
		return 1;
	}
	int busytft = 0;
#if DEBUG
	printf("Set the busyness of the LCD, 1 when the lcd is busy and 0 if the lcd is free.\n");
	while(k_msgq_get(&lcdMsg, &busytft, K_FOREVER));
	while (status && busytft)
	{
		printf("Set the busyness of the LCD, 1 when the lcd is busy and 0 if the lcd is free.\n");
		while(k_msgq_get(&lcdMsg, &busytft, K_FOREVER));
	}
	
#else
	while((status) && (gpio_pin_get_dt(&busytft)));
#endif
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
		for(uint8_t i = 0; i < NUMBERS_OF_SLAVES; i++)
		{
			// otherStatuses &&= getSlaveStatus(i);
		}
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


float rawVoltageToRealVoltage(uint8_t index)
{
	/*
	((Vreal * R203 * 12) / ((R203 + R202) * 27)) + Vref = Vmcu
	(Vreal * 5640)/(4,062,690) = Vmcu - Vref
	Vreal = (Vmcu - Vref) * (4062690) / (5640)
	*/
// divider gain : 0.00312354
//Iso gain : 7.5
//AOP Vref + vin*0.44
 
 //RV =  ( AV - Vref )* 97.015

	int32_t raw = readAdc(index);
	float real = (raw - VREF) * 97.015;
	return real;
}

float rawCurrentToRealCurrent(uint8_t index)
{
	/*
	((Ireal * 22 * 12) / ( 200 * 27)) + Vref = Vmcu
	((Ireal * 264) / 5400)) + Vref = Vmcu
	Ireal = (Vmcu - Vref) * 5400 / 264
	*/
	int32_t raw = readAdc(index);
	float real = (raw - VREF) * 20.4545;
	return real;
}


float rawTempToRealTemp(uint8_t index)
{
	float R , T ;
	int32_t raw = readAdc(index);
	R = (raw*10000)/(3300 - raw);
	T =  ( log(R) - log(10000) + (3450*(1/(25+273))) ) / 3450;
	return (T - 273);
	
	//T =  ( ln(R) - ln(Ro) + B*(1/To) ) / B

	// (3300)*R/(20000+R) = raw voltages are in mili volt, resistors are in Kohm.
	// 3300*R = (20000*raw)+(R*raw)
	//(3300 - raw)*R = 20 * raw
	//R = (20 * raw)/(3300 - raw)


	//R = R0 * exp(B * (1/T) - (1/T0))
	//T0 = 273 + 25, R0 = 10000, B = 3450
	//logR = logR0 + B/T - B/T0 
	//logR - logR0 + B/T0 = B/T
	//T = B/(logR - logR0 + T0)
	
}

