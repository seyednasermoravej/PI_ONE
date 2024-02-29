/*
 * Copyright (c) 2021 LAAS-CNRS
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU Lesser General Public License as published by
 *   the Free Software Foundation, either version 2.1 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Lesser General Public License for more details.
 *
 *   You should have received a copy of the GNU Lesser General Public License
 *   along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * SPDX-License-Identifier: LGLPV2.1
 */

/**
 * @author  Jean Alinei <jean.alinei@laas.fr>
 * @author  Clément Foucher <clement.foucher@laas.fr>
 * @author  Antoine Boche <antoine.boche@laas.fr>
 */

/////
// Include

// Zephyr

// Stdlib
#include <stdio.h>

// Current file header
#include "../inc/opalib_control_pid.h"


/////
// Defines

// Saturation values used for the PWM duty cycle
#define LOW_DUTY 0.1
#define HIGH_DUTY 0.9
#define VMAX 130
#define IMAX_INT 16
#define IMAX_LEG 8

/////
// Local variables

static float32_t saturation_interleaved, saturation_leg1, saturation_leg2, saturation_motor; 			// Represent the minimum voltage required for the entry to suit physical limits

// Interleaved PID variables------------------------------------------------------------------------------------------------------
static arm_pid_instance_f32 PID_variables;  // PID structure
static float32_t error_pid;					// PID error
static uint32_t pid_period_us; 			  	// Period duration of the PID calculation loop in µs (used for Ki calculation)

static float32_t prev_pid_out = 0.1; 		// Stores the previous unsaturated output
static float32_t pid_out;        			// Stores the current pid_output after saturation and anti windUp (the effective duty cycle)

// Anti-Windup variables
static float32_t pid_out_windUp = 0.1;     	// Stores the current pid output after anti windup and before saturation
static float32_t WindUp_sub, WindUp_mult; 	// Transition variables for arm calculation of the WindUp
float32_t Kw = 0.000143; 					// WindUp Parameter

// PWM variable
static float32_t pwm_duty_cycle = 0.1; 			// PWM initialization duty cycle value for the interleaved mode

// Saturation Variables
static float32_t saturation_pid_interleaved_low = 0;
static float32_t saturation_pid_interleaved_high = 1;
//--------------------------------------------------------------------------------------------------------------------------------


// Leg1 PID variables--------------------------------------------------------------------------------------------------------------
static arm_pid_instance_f32 PID_1_variables;    // PID structure
static float32_t error_pid_1;					// PID error

static float32_t prev_pid_1_out = 0.1; 		// Stores the previous unsaturated output
static float32_t pid_1_out;        			// Stores the current pid_output after saturation and anti windUp (the effective duty cycle)

// Anti-Windup variables
static float32_t pid_1_out_windUp = 0.1;     	// Stores the current pid output after anti windup and before saturation
static float32_t WindUp_1_sub, WindUp_1_mult; 	// Transition variables for arm calculation of the WindUp
float32_t Kw_1 = 0.000143; 					    // WindUp Parameter

// PWM variable
static float32_t pwm_duty_cycle_1 = 0.1; 		// PWM initialization duty cycle value for leg 1

// Saturation Variables
static float32_t saturation_pid_leg1_low = 0;
static float32_t saturation_pid_leg1_high = 1;
//--------------------------------------------------------------------------------------------------------------------------------


// Leg2 PID variables--------------------------------------------------------------------------------------------------------------
static arm_pid_instance_f32 PID_2_variables;    // PID structure
static float32_t error_pid_2;					// PID error

static float32_t prev_pid_2_out = 0.1; 		// Stores the previous unsaturated output
static float32_t pid_2_out;        			// Stores the current pid_output after saturation and anti windUp (the effective duty cycle)

// Anti-Windup variables
static float32_t pid_2_out_windUp = 0.1;     	// Stores the current pid output after anti windup and before saturation
static float32_t WindUp_2_sub, WindUp_2_mult; 	// Transition variables for arm calculation of the WindUp
float32_t Kw_2 = 0.000143;                      // WindUp Parameter

// PWM variable
static float32_t pwm_duty_cycle_2 = 0.1; 		// PWM initialization duty cycle value for leg 2

// Saturation Variables
static float32_t saturation_pid_leg2_low = 0;
static float32_t saturation_pid_leg2_high = 1;

//--------------------------------------------------------------------------------------------------------------------------------



// motor PID variables--------------------------------------------------------------------------------------------------------------
static arm_pid_instance_f32 PID_motor_variables;    // PID structure
static float32_t error_pid_motor;					// PID error
static uint32_t pid_motor_period_us; 			  	// Period duration of the PID calculation loop in µs (used for Ki calculation)

static float32_t prev_pid_motor_out = 0.1; 		// Stores the previous unsaturated output
static float32_t pid_motor_out;        			// Stores the current pid_output after saturation and anti windUp (the effective duty cycle)

// Anti-Windup variables
static float32_t pid_motor_out_windUp = 0.1;     	// Stores the current pid output after anti windup and before saturation
static float32_t WindUp_motor_sub, WindUp_motor_mult; 	// Transition variables for arm calculation of the WindUp
float32_t Kw_motor = 0.000143; 					    // WindUp Parameter

// PWM variable
static float32_t pwm_duty_cycle_motor = 0.1; 		// PWM initialization duty cycle value for leg 2

// Saturation variables
static float32_t saturation_pid_motor_low = 0;
static float32_t saturation_pid_motor_high = 1;
//--------------------------------------------------------------------------------------------------------------------------------


/////
// Public Functions

/**
 * This function initializes all the parameters
 * needed for the PID calculation for the buck topology
 */
void opalib_control_init_interleaved_pid(float32_t kp, float32_t ki, float32_t kd, uint32_t task_period_us)
{
	pid_period_us = task_period_us;

	float32_t million = 1000000;
	PID_variables.Kp = kp;
	PID_variables.Ki = ki * (pid_period_us / million);
	PID_variables.Kd = kd;

	arm_pid_init_f32(&PID_variables, 1);
	saturation_interleaved = VMAX;
}

/**
 * This function initializes all the parameters
 * needed for the PID calculation for the buck topology
 */
void opalib_control_init_leg1_pid(float32_t kp, float32_t ki, float32_t kd, uint32_t task_period_us)
{
	pid_period_us = task_period_us;

	float32_t million = 1000000;
	PID_1_variables.Kp = kp;
	PID_1_variables.Ki = ki * (pid_period_us / million);
	PID_1_variables.Kd = kd;

	arm_pid_init_f32(&PID_1_variables, 1);
	saturation_leg1 = VMAX;

}

/**
 * This function initializes all the parameters
 * needed for the PID calculation for the buck topology
 */
void opalib_control_init_leg2_pid(float32_t kp, float32_t ki, float32_t kd, uint32_t task_period_us)
{
	pid_period_us = task_period_us;

	float32_t million = 1000000;
	PID_2_variables.Kp = kp;
	PID_2_variables.Ki = ki * (pid_period_us / million);
	PID_2_variables.Kd = kd;

	arm_pid_init_f32(&PID_2_variables, 1);
	saturation_leg2 = VMAX;

}

/**
 * This function initializes all the parameters
 * needed for the PID calculation for the buck topology
 */
void opalib_control_init_motor_pid(float32_t kp, float32_t ki, float32_t kd, uint32_t motor_control_period_us, float32_t saturation_high, float32_t saturation_low)
{
	pid_motor_period_us = motor_control_period_us;

	float32_t million = 1000000;
	PID_motor_variables.Kp = kp;
	PID_motor_variables.Ki = ki * (pid_motor_period_us / million);
	PID_motor_variables.Kd = kd;

	arm_pid_init_f32(&PID_motor_variables, 1);
	saturation_motor = IMAX_INT;

}


/**
 * This function update the voltage reference of the
 * control task.
 */

void opalib_control_update_saturation_interleaved(float32_t new_saturation_value)
{
	saturation_interleaved = new_saturation_value;
}

/**
 * This function update the voltage reference of the
 * control task.
 */

void opalib_control_update_saturation_leg1(float32_t new_saturation_value)
{
	saturation_leg1 = new_saturation_value;
}


/**
 * This function update the voltage reference of the
 * control task.
 */
void opalib_control_update_saturation_leg2(float32_t new_saturation_value)
{
	saturation_leg2 = new_saturation_value;
}

/**
 * This function update the voltage reference of the
 * control task.
 */

void opalib_control_update_saturation_motor(float32_t new_saturation_value)
{
	saturation_motor = new_saturation_value;
}


/**
 * This function does the PID calculation
 * The PID has an anti-windup that permits to avoid
 * loosing control.
 */

float32_t opalib_control_interleaved_pid_calculation(float32_t reference, float32_t measurement){

	if (measurement <= saturation_interleaved){

		arm_sub_f32(&reference, &measurement, &error_pid, 1);		// CALCULATING THE ERROR BASED ON THE REFERENCE
		pid_out = arm_pid_f32(&PID_variables, error_pid);	        // PID CALCULATIONS

		if(pid_out>saturation_pid_interleaved_high){
			pid_out=saturation_pid_interleaved_high;
		}else if(pid_out<saturation_pid_interleaved_low){
			pid_out=saturation_pid_interleaved_low;
		}
		//PID anti WindUp saturation
		arm_sub_f32(&pwm_duty_cycle, &prev_pid_out, &WindUp_sub, 1);
		arm_mult_f32(&WindUp_sub, &Kw, &WindUp_mult, 1);
		arm_add_f32(&WindUp_mult, &pid_out, &pid_out_windUp, 1);

		PID_variables.state[2] = pid_out_windUp;
		pwm_duty_cycle = pid_out_windUp;
		prev_pid_out = pid_out;
	}

	return pwm_duty_cycle;
}

/**
 * This function does the PID_1 calculation
 * The PID has an anti-windup that permits to avoid
 * loosing control.
 */

float32_t opalib_control_leg1_pid_calculation(float32_t reference, float32_t measurement)
{
	if (measurement <= saturation_leg1){

		arm_sub_f32(&reference, &measurement, &error_pid_1, 1);		// CALCULATING THE ERROR BASED ON THE REFERENCE

		pid_1_out = arm_pid_f32(&PID_1_variables, error_pid_1);	        // PID CALCULATIONS

		if(pid_1_out>saturation_pid_leg1_high){
			pid_1_out=saturation_pid_leg1_high;
		}else if(pid_1_out<saturation_pid_leg1_low){
			pid_1_out=saturation_pid_leg1_low;
		}

		//PID anti WindUp saturation
		arm_sub_f32(&pwm_duty_cycle_1, &prev_pid_1_out, &WindUp_1_sub, 1);
		arm_mult_f32(&WindUp_1_sub, &Kw_2, &WindUp_1_mult, 1);
		arm_add_f32(&WindUp_1_mult, &pid_1_out, &pid_1_out_windUp, 1);

		PID_1_variables.state[2] = pid_1_out_windUp;
		pwm_duty_cycle_1 = pid_1_out_windUp;
		prev_pid_1_out = pid_1_out;
	}

	return pwm_duty_cycle_1;
}


/**
 * This function does the PID_2 calculation
 * The PID has an anti-windup that permits to avoid
 * loosing control.
 */

float32_t opalib_control_leg2_pid_calculation(float32_t reference, float32_t measurement)
{
	if (measurement <= saturation_leg2){

		arm_sub_f32(&reference, &measurement, &error_pid_2, 1);		// CALCULATING THE ERROR BASED ON THE REFERENCE

		pid_2_out = arm_pid_f32(&PID_2_variables, error_pid_2);	        // PID CALCULATIONS

		if(pid_2_out>saturation_pid_leg2_high){
			pid_2_out=saturation_pid_leg2_high;
		}else if(pid_2_out<saturation_pid_leg2_low){
			pid_2_out=saturation_pid_leg2_low;
		}

		//PID anti WindUp saturation
		arm_sub_f32(&pwm_duty_cycle_2, &prev_pid_2_out, &WindUp_2_sub, 1);
		arm_mult_f32(&WindUp_2_sub, &Kw_2, &WindUp_2_mult, 1);
		arm_add_f32(&WindUp_2_mult, &pid_2_out, &pid_2_out_windUp, 1);

		PID_2_variables.state[2] = pid_2_out_windUp;
		pwm_duty_cycle_2 = pid_2_out_windUp;
		prev_pid_2_out = pid_2_out;
	}

	return pwm_duty_cycle_2;
}

/**
 * This function does the PID calculation
 * The PID has an anti-windup that permits to avoid
 * loosing control.
 */

float32_t opalib_control_motor_pid_calculation(float32_t reference, float32_t measurement){

	if (measurement <= saturation_motor){

		arm_sub_f32(&reference, &measurement, &error_pid_motor, 1);		// CALCULATING THE ERROR BASED ON THE REFERENCE
		pid_motor_out = arm_pid_f32(&PID_motor_variables, error_pid_motor);	        // PID CALCULATIONS

		if(pid_motor_out>saturation_pid_motor_high){
			pid_motor_out=saturation_pid_motor_high;
		}else if(pid_motor_out<saturation_pid_motor_low){
			pid_motor_out=saturation_pid_motor_low;}

		//PID anti WindUp saturation
		arm_sub_f32(&pwm_duty_cycle_motor, &prev_pid_motor_out, &WindUp_motor_sub, 1);
		arm_mult_f32(&WindUp_motor_sub, &Kw_motor, &WindUp_motor_mult, 1);
		arm_add_f32(&WindUp_motor_mult, &pid_motor_out, &pid_motor_out_windUp, 1);

		PID_motor_variables.state[2] = pid_motor_out_windUp;
		pwm_duty_cycle_motor = pid_motor_out_windUp;
		prev_pid_motor_out = pid_motor_out;
		pwm_duty_cycle_motor = pid_motor_out;
	}

	return pwm_duty_cycle_motor;
}


void opalib_control_motor_pid_kp_update(float32_t new_kp){
		PID_motor_variables.Kp = new_kp;
}

void opalib_control_motor_pid_ki_update(float32_t new_ki){
		PID_motor_variables.Ki = new_ki;
}


/**
 * This function resets the states of the pid
 */
void opalib_control_pid_reset_state()
{
	arm_pid_reset_f32(&PID_variables);
	arm_pid_reset_f32(&PID_1_variables);
	arm_pid_reset_f32(&PID_2_variables);
	arm_pid_reset_f32(&PID_motor_variables);
}
