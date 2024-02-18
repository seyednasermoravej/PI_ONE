/*
 * Copyright (c) 2016 Intel Corporation
 * Copyright (c) 2020 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file Sample app to demonstrate PWM.
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/device.h>
#include <zephyr/drivers/pwm.h>

static const struct pwm_dt_spec pwm_led0 = PWM_DT_SPEC_GET(DT_ALIAS(pwmt));
static const struct pwm_dt_spec pwm_led1 = PWM_DT_SPEC_GET(DT_ALIAS(pwmn));
// static const struct pwm_dt_spec pwm_led2 = PWM_DT_SPEC_GET(DT_ALIAS(pwmtest));

#define MIN_PERIOD PWM_SEC(1U) / 128U
#define DUTYCYCLE0	0.7
#define DUTYCYCLE1	0.3
#define DUTYCYCLE2	0.5
#define MAX_PERIOD 10000
/*adc*/
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/adc.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/util.h>

#if !DT_NODE_EXISTS(DT_PATH(zephyr_user)) || \
	!DT_NODE_HAS_PROP(DT_PATH(zephyr_user), io_channels)
#error "No suitable devicetree overlay specified"
#endif

#define DT_SPEC_AND_COMMA(node_id, prop, idx) \
	ADC_DT_SPEC_GET_BY_IDX(node_id, idx),

/* Data of ADC io-channels specified in devicetree. */
static const struct adc_dt_spec adc_channels[] = {
	DT_FOREACH_PROP_ELEM(DT_PATH(zephyr_user), io_channels,
			     DT_SPEC_AND_COMMA)
};

/*adc*/

/*led*/

#include <zephyr/drivers/gpio.h>
/* The devicetree node identifier for the "led0" alias. */
#define LED0_NODE DT_ALIAS(led0)

/*
 * A build error on this line means your board is unsupported.
 * See the sample documentation for information on how to fix this.
 */
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);

/*led*/


int main(void)
{
	/*led*/
		int ret;
	bool led_state = true;

	if (!gpio_is_ready_dt(&led)) {
		return 0;
	}

	ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
	if (ret < 0) {
		return 0;
	}

	/*led*/



	volatile uint32_t max_period;
	uint32_t period;
	uint8_t dir = 0U;

	printk("PWM-based blinky\n");

	if (!pwm_is_ready_dt(&pwm_led0)) {
		printk("Error: PWM device %s is not ready\n",
		       pwm_led0.dev->name);
		return 0;
	}
	if (!pwm_is_ready_dt(&pwm_led1)) {
		printk("Error: PWM device %s is not ready\n",
		       pwm_led1.dev->name);
		return 0;
	}
	// if (!pwm_is_ready_dt(&pwm_led2)) {
	// 	printk("Error: PWM device %s is not ready\n",
	// 	       pwm_led2.dev->name);
	// 	return 0;
	// }

	/*
	 * In case the default MAX_PERIOD value cannot be set for
	 * some PWM hardware, decrease its value until it can.
	 *
	 * Keep its value at least MIN_PERIOD * 4 to make sure
	 * the sample changes frequency at least once.
	 */
	printk("Calibrating for channel %d...\n", pwm_led0.channel);
	max_period = MAX_PERIOD;
	uint32_t duty0 = (uint32_t) max_period * DUTYCYCLE0;
	while (pwm_set_dt(&pwm_led0, max_period, duty0)) {
		max_period /= 2U;
		if (max_period < (4U * MIN_PERIOD)) {
			printk("Error: PWM device "
			       "does not support a period at least %lu\n",
			       4U * MIN_PERIOD);
			return 0;
		}
	}

	printk("Done calibrating; maximum/minimum periods %u/%lu nsec\n",
	       max_period, MIN_PERIOD);

	printk("Calibrating for channel %d...\n", pwm_led1.channel);
	max_period = MAX_PERIOD;
	while (pwm_set_dt(&pwm_led1, max_period, (uint32_t)(max_period * DUTYCYCLE1))) {
		max_period /= 2U;
		if (max_period < (4U * MIN_PERIOD)) {
			printk("Error: PWM device "
			       "does not support a period at least %lu\n",
			       4U * MIN_PERIOD);
			return 0;
		}
	}
	printk("Calibrating for channel %d...\n", pwm_led1.channel);
	max_period = MAX_PERIOD;
	// while (pwm_set_dt(&pwm_led2, max_period, (uint32_t)(max_period * DUTYCYCLE2))) {
	// 	max_period /= 2U;
	// 	if (max_period < (4U * MIN_PERIOD)) {
	// 		printk("Error: PWM device "
	// 		       "does not support a period at least %lu\n",
	// 		       4U * MIN_PERIOD);
	// 		return 0;
	// 	}
	// }



	// pwm_enable_capture
	// pwm_enable
	period = max_period;

	/*adc*/


	int err;
	uint32_t count = 0;
	uint16_t buf;
	struct adc_sequence sequence = {
		.buffer = &buf,
		/* buffer size in bytes, not number of samples */
		.buffer_size = sizeof(buf),
	};

	/* Configure channels individually prior to sampling. */
	for (size_t i = 1; i < ARRAY_SIZE(adc_channels); i++) {
		if (!adc_is_ready_dt(&adc_channels[i])) {
			printk("ADC controller device %s not ready\n", adc_channels[i].dev->name);
			return 0;
		}

	volatile int er = adc_channel_setup_dt(&adc_channels[i]);
		if (er < 0) {
			printk("Could not setup channel #%d (%d)\n", i, er);
			return 0;
		}
	}

	while (1) {
		printk("ADC reading[%u]:\n", count++);
		for (int i = 1; i < ARRAY_SIZE(adc_channels); i++) {
			volatile int32_t val_mv;

			printk("- %s, channel %d: ",
			       adc_channels[i].dev->name,
			       adc_channels[i].channel_id);

			(void)adc_sequence_init_dt(&adc_channels[i], &sequence);

			err = adc_read_dt(&adc_channels[i], &sequence);
			if (err < 0) {
				printk("Could not read (%d)\n", err);
				continue;
			}

			/*
			 * If using differential mode, the 16 bit value
			 * in the ADC sample buffer should be a signed 2's
			 * complement value.
			 */
			if (adc_channels[i].channel_cfg.differential) {
				val_mv = (int32_t)((int16_t)buf);
			} else {
				val_mv = (int32_t)buf;
			}
			printk("%"PRId32, val_mv);
			err = adc_raw_to_millivolts_dt(&adc_channels[i],
						       &val_mv);
			/* conversion to mV may not be supported, skip if not */
			if (err < 0) {
				printk(" (value in mV not available)\n");
			} else {
				printk(" = %"PRId32" mV\n", val_mv);
			}
		}
/*led*/
		ret = gpio_pin_toggle_dt(&led);
		if (ret < 0) {
			return 0;
		}
/*led*/
		k_sleep(K_MSEC(1000));
	}

	/*adc*/
	while (1) {
		// ret = pwm_set_dt(&pwm_led0, period, period / 2U);
		// if (ret) {
		// 	printk("Error %d: failed to set pulse width\n", ret);
		// 	return 0;
		// }

		// period = dir ? (period * 2U) : (period / 2U);
		// if (period > max_period) {
		// 	period = max_period / 2U;
		// 	dir = 0U;
		// } else if (period < MIN_PERIOD) {
		// 	period = MIN_PERIOD * 2U;
		// 	dir = 1U;
		// }

		k_sleep(K_SECONDS(4U));
	}
	return 0;
}