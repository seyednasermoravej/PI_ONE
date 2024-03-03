#include "../inc/adcs.h"

#ifdef MICRO

#if !DT_NODE_EXISTS(DT_PATH(zephyr_user)) || \
	!DT_NODE_HAS_PROP(DT_PATH(zephyr_user), io_channels)
#error "No suitable devicetree overlay specified"
#endif

#define ADC_DT_SPEC_AND_COMMA(node_id, prop, idx) \
	ADC_DT_SPEC_GET_BY_IDX(node_id, idx),

/* Data of ADC io-channels specified in devicetree. */
static const struct adc_dt_spec adc_channels[] = {
	DT_FOREACH_PROP_ELEM(DT_PATH(zephyr_user), io_channels,
			     ADC_DT_SPEC_AND_COMMA)
};
LOG_MODULE_REGISTER(adcs, LOG_LEVEL_DBG);

#endif
int initAdcs()
{
#ifdef MICRO
	/* Configure channels individually prior to sampling. */
	for (size_t i = 0; i < TEMP_MCU_IDX; i++) {
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

#endif
	LOG_INF("ADC initialization finished.\n");
}


uint16_t readAdc(uint8_t index)
{
#ifdef MICRO
	uint32_t count = 0;
	uint16_t buf;
	struct adc_sequence sequence = {
		.buffer = &buf,
		/* buffer size in bytes, not number of samples */
		.buffer_size = sizeof(buf),
	};

	int32_t val_mv;

	LOG_INF("- %s, channel %d: ",
			adc_channels[index].dev->name,
			adc_channels[index].channel_id);

	(void)adc_sequence_init_dt(&adc_channels[index], &sequence);

	int err = adc_read_dt(&adc_channels[index], &sequence);
	if (err < 0) {
		LOG_INF("Could not read (%d)\n", err);
	}

	val_mv = (int32_t)buf;

	LOG_INF("%"PRId32, val_mv);
	err = adc_raw_to_millivolts_dt(&adc_channels[index],
						&val_mv);
	/* conversion to mV may not be supported, skip if not */
		if (err < 0) {
			printk(" (value in mV not available)\n");
		} else {
			printk(" = %"PRId32" mV\n", val_mv);
		}
    
#else
	return 10;
#endif
}