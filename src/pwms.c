#ifndef __PWMS__H_
#define __PWMS__H_

#include "../inc/pwms.h"

#if !DT_NODE_EXISTS(DT_PATH(pwmleds, allpwms)) || \
	!DT_NODE_HAS_PROP(DT_PATH(pwmleds, allpwms), pwms)
#error "No suitable devicetree overlay specified"
#endif

#define DT_SPEC_AND_COMMA(node_id, prop, idx) \
	PWM_DT_SPEC_GET_BY_IDX(node_id, idx),

/* Data of ADC io-channels specified in devicetree. */
static const struct pwm_dt_spec pwm_channels[] = {
	DT_FOREACH_PROP_ELEM(DT_NODELABEL(allpwms), pwms,
			     DT_SPEC_AND_COMMA)
};
LOG_MODULE_REGISTER(pwms, LOG_LEVEL_DBG);


void initPwms()
{
	/* Configure channels individually prior to sampling. */
	for (size_t i = 0; i < ARRAY_SIZE(pwm_channels); i++) {
        if (!pwm_is_ready_dt(&pwm_channels[i])) {
            printk("Error: PWM device %s is not ready\n",
                pwm_channels[i].dev->name);
            return;
        }
    }
	LOG_INF("PWM initialization finished.\n");
}


void pwmSet(uint8_t idx, uint32_t frequency, float dutycycle)
{
    uint32_t period = 1000000000/frequency; 
    uint32_t pulse = (1000000000 * dutycycle) / frequency;
	int err = pwm_set_dt(&pwm_channels[idx], period, pulse);
    if (err < 0)
        return;
    LOG_INF("The PWM is set to frequency: %u, pulse: %u.\n", frequency, pulse);
}

void turnOffAllPWMs()
{
	for (size_t i = 0; i < ARRAY_SIZE(pwm_channels); i++)
    {
        int err = pwm_set_dt(&pwm_channels[i], 10000, 0);
            LOG_INF("PWM device %s turned off.\n",
                pwm_channels[i].dev->name);
    }
    LOG_INF("All pwm channels are turned off.\n");

}
#endif