#include "../inc/leds.h"

/*led*/
#ifdef MICRO
/*
 * A build error on this line means your board is unsupported.
 * See the sample documentation for information on how to fix this.
 */
LOG_MODULE_REGISTER(leds, LOG_LEVEL_DBG);
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);

#endif
/*led*/


int initLeds()
{

#ifdef MICRO
	/*led*/
	int ret;

	if (!gpio_is_ready_dt(&led)) {
		return 0;
	}

	ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
	if (ret < 0) {
		return 0;
	}

/*led*/
#endif
	LOG_INF("LEDs initialization finished.\n");
	return 0;
}

int ledToggle()
{
#ifdef MICRO
	int ret = 0;
	ret = gpio_pin_toggle_dt(&led);
	if (ret < 0) {
		return 0;
	}
#endif
	LOG_INF("Led toggled.\n");
	return 0;
}