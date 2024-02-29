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

#ifndef OPALIB_CONTROL_H_
#define OPALIB_CONTROL_H_

#include <zephyr/kernel.h>
#include <arm_math.h>   // adds all the CMSIS library

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief     This function initialize all the parameters
 * needed for the PID calculation for the buck topology
 *
 * @param[in] kp        Proportional gain for PID calculation
 * @param[in] ki        Integral gain for PID calculation
 * @param[in] kd        Derivative gain for PID calculation
 * @param[in] task_period_us the period of the control task in micro-seconds
 */
void opalib_control_init_interleaved_pid(float32_t kp, float32_t ki, float32_t kd, uint32_t task_period_us);

/**
 * @brief     This function initialize all the parameters for leg 1
 *
 * @param[in] kp        Proportional gain for PID calculation
 * @param[in] ki        Integral gain for PID calculation
 * @param[in] kd        Derivative gain for PID calculation
 * @param[in] task_period_us the period of the control task in micro-seconds
 */
void opalib_control_init_leg1_pid(float32_t kp, float32_t ki, float32_t kd, uint32_t task_period_us);

/**
 * @brief     This function initialize all the parameters for leg 2
 *
 * @param[in] kp        Proportional gain for PID calculation
 * @param[in] ki        Integral gain for PID calculation
 * @param[in] kd        Derivative gain for PID calculation
 * @param[in] task_period_us the period of the control task in micro-seconds
 */
void opalib_control_init_leg2_pid(float32_t kp, float32_t ki, float32_t kd, uint32_t task_period_us);


/**
 * @brief     This function initialize all the parameters for leg 2
 *
 * @param[in] kp        Proportional gain for PID calculation
 * @param[in] ki        Integral gain for PID calculation
 * @param[in] kd        Derivative gain for PID calculation
 * @param[in] task_period_us the period of the control task in micro-seconds
 */
void opalib_control_init_motor_pid(float32_t kp, float32_t ki, float32_t kd, uint32_t motor_control_period_us, float32_t saturation_high, float32_t saturation_low);

/**
 * @brief     This function update the saturation value used to protect both legs in the pid calculation
 * 
 * @param[in] new_saturation_value      The new saturation value to be set
 */
void opalib_control_update_saturation_interleaved(float32_t new_saturation_value);


/**
 * @brief     This function update the saturation value used to protect leg1 in the pid calculation
 * 
 * @param[in] new_saturation_value      The new saturation value to be set
 */
void opalib_control_update_saturation_leg1(float32_t new_saturation_value);

/**
 * @brief     This function update the saturation value used to protect leg2 in the pid calculation
 * 
 * @param[in] new_saturation_value      The new saturation value to be set
 */
void opalib_control_update_saturation_leg2(float32_t new_saturation_value);


/**
 * @brief     This function calculates the error and calls a single pid for both legs to get an updated value for the duty cycle
 * 
 * @param[in] voltage_reference    floating point reference value to be tracked
 * @param[in] measurement   floating point measurement of the value used to calculate the error
 * 
 * @return  duty cycle to be sent to HRTIM peripheral
 */

float32_t opalib_control_interleaved_pid_calculation(float32_t reference, float32_t measurement);


/**
 * @brief     This function calculates the error and calls pid 1 to get an updated value for the duty cycle
 * 
 * @param[in] voltage_reference    floating point reference value to be tracked
 * @param[in] measurement   floating point measurement of the value used to calculate the error
 * 
 * @return  duty cycle to be sent to HRTIM peripheral
 */

float32_t opalib_control_leg1_pid_calculation(float32_t reference, float32_t measurement);

/**
 * @brief     This function calculates the error and calls pid 2 to get an updated value for the duty cycle
 * 
 * @param[in] voltage_reference    floating point reference value to be tracked
 * @param[in] measurement   floating point measurement of the value used to calculate the error
 * 
 * @return  duty cycle to be sent to HRTIM peripheral
 */

float32_t opalib_control_leg2_pid_calculation(float32_t reference, float32_t measurement);


/**
 * @brief     This function calculates the error and calls motor pid to get an updated value for the duty cycle
 * 
 * @param[in] voltage_reference    floating point reference value to be tracked
 * @param[in] measurement   floating point measurement of the value used to calculate the error
 * 
 * @return  duty cycle to be sent to HRTIM peripheral
 */
float32_t opalib_control_motor_pid_calculation(float32_t reference, float32_t measurement);



/**
 * @brief     This function updates the kp value for the pid control of the motor
 * 
 * @param[in] new_kp    floating point reference value to be tracked
 */
void opalib_control_motor_pid_kp_update(float32_t new_kp);

/**
 * @brief     This function updates the ki value for the pid control of the motor
 * 
 * @param[in] new_ki    floating point reference value to be tracked
 */
void opalib_control_motor_pid_ki_update(float32_t new_ki);


/**
 * @brief This function reset the pid state
 */
void opalib_control_pid_reset_state(void);




#ifdef __cplusplus
}
#endif

#endif // OPALIB_CONTROL_H_
