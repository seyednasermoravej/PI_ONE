/*
 * Copyright (c) 2016 Intel Corporation
 * Copyright (c) 2020 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file Sample app to demonstrate PWM.
 */

#include "../inc/main.h"
#include "../inc/leds.h"
#include "../inc/adcs.h"
#include "../inc/pwms.h"


#ifdef MICRO
LOG_MODULE_REGISTER(main, LOG_LEVEL_DBG);

#if !DT_NODE_EXISTS(DT_NODELABEL(busylcd))
#error "Overlay for busytft node not properly defined."
#endif

static const struct gpio_dt_spec busytft =
	GPIO_DT_SPEC_GET_OR(DT_NODELABEL(busylcd), gpios, {0});


#endif

void initBusyTft()
{
#ifdef MICRO
	if(!gpio_is_ready_dt(&busytft))
	{
		LOG_INF("The gpio is not ready.\n");
		return;
	}
#endif
	LOG_INF("The gpio is ready to use.\n");
}

void initBoard()
{
	initLeds();

	initBusyTft();

	initAdcs();

	initPwms();

	// initCan();

	// initModbus();
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
	// #ifdef PC
	// scanf("%d", ovRegulate);
	// #endif
	// LOG_INF("Please enter the minimum and maximum output voltage values for battery protection.\n");
	// #ifdef PC
	// scanf("%d %d", ovMinBattery, ovMaxBattery);
	// #endif
	// LOG_INF("Please enter the minimum and maximum input voltage values for battery protection.\n");
	// #ifdef PC
	// scanf("%d %d", inMinBattery, inMaxBattery);
	// #endif
	// LOG_INF("Please enter max charge and discharge battery current.\n");
	// #ifdef PC
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
#ifdef MICRO 
	uint8_t mode = 0;
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
#ifdef MICRO
	uint16_t vBatCharge, vBatDischarge, inputVoltage;
	inputVoltage = readAdc(VIN_IDX);
	//vbatCharge and discharge must be provided by user.
#else
	int vBatCharge, vBatDischarge, inputVoltage;
	printf("Enter voltage baterry charge, discharge and input voltage with space between them.\n");
	scanf("%d %d %d", &vBatCharge, &vBatDischarge, &inputVoltage);
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
#ifdef PC
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
#ifdef PC
	printf("What are the values of the following? enter with a sapce between them:\n");
	printf("Now these values are given by the tester user to check the logic of the program.\n");
	printf("In real application these values will be read by the ADC.\n");
	printf("temp temp2 temp_mcu vin vout iIn iOut.\n");
	// int temp, temp2, tempMcu, vIn, vOut, iIn, iOut;
	int temp, temp2, tempMcu, vIn, vOut, iIn, iOut;
	scanf("%d %d %d %d %d %d %d", &temp, &temp2, &tempMcu, &vIn, &vOut, &iIn, &iOut);
#else
	uint16_t temp = readAdc(TEMP_IDX);
	uint16_t temp2 = readAdc(TEMP2_IDX);
	uint16_t tempMcu = readAdc(TEMP_MCU_IDX);
	uint16_t vIn = readAdc(VIN_IDX);
	uint16_t vOut = readAdc(VOUT_IDX);
	uint16_t iIn = readAdc(I_IN_IDX);
	uint16_t iOut = readAdc(I_OUT_IDX);
#endif

	LOG_INF("Temp value is: %d\n", temp);
	LOG_INF("temp2 value is: %d\n", temp2);
	LOG_INF("temp mcu value is: %d\n", tempMcu);
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
	int userSelection;
#ifdef PC
	printf("Enter user selection.\n");
	scanf("%d", &userSelection);
#endif
	LOG_INF("The user has selected option %u.\n", userSelection);
	return userSelection;
}

int main(void)
{

	initBoard();

	bool status = initialCheckSequence();

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
#ifdef MICRO
	while((status) && (gpio_pin_get_dt(&busytft)));
#else
	int busytft = 1;
	while((status) && (busytft))
	{
		printf("Set the busyness of the LCD, 1 when the lcd is busy and 0 if the lcd is free.\n");
		scanf("%d", &busytft);
	}
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