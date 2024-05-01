#include "../inc/adcs.h"


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

int initAdcs()
{
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

	LOG_INF("ADC initialization finished.\n");
}


static int32_t readAdc(uint8_t index)
{
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
	return val_mv;
    
}



float realTemp(uint8_t index)
{
	float R , T ;
	float T0 = (273 + 25);
	float R0 = 10000;
	float B = 3450;
	int32_t raw = readAdc(index);
	R = ( (raw*20000)/(3300 - raw) ); 
	T =  B/( log(R) - log(R0) + (B/T0));
	return (T - 273);
	
	//T =  ( ln(R) - ln(Ro) + B*(1/To) ) / B

	// (3300)*R/(20000+R) = raw voltages are in mili volt, resistors are in ohm.
	// 3300*R = (20000*raw)+(R*raw)
	//(3300 - raw)*R = 20000 * raw
	//R = (20000 * raw)/(3300 - raw)


	//R = R0 * exp(B * (1/T) - (1/T0))
	//T0 = 273 + 25, R0 = 10000, B = 3450
	//logR = logR0 + B/T - B/T0 
	//logR - logR0 + B/T0 = B/T
	//T = B/(logR - logR0 + B/T0)
	
}



float realCurrent(uint8_t index)
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


float realVoltage(uint8_t index)
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
	//float real = (raw - VREF) * 97.015;
	float real = (raw - 2600) * 76.92;

	return real;
}

float realVoltageN(uint8_t index)
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
	float real = (raw - 2) * 71.43;
	//float real = (raw - VREF) * 97.015;
	
	return real;
}

float fanSpeed()
{
	int32_t fan;
	fan = readAdc(FAN_IN_IDX);
	LOG_INF("Fan value is: %u.", fan);
}