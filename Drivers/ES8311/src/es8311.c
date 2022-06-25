/**
 * @file es8311.c
 * @author Gonzalo E. Sanchez (gonzalo.e.sds@gmail.com)
 * @brief ES8311 driver implementation.
 * @version 0.1
 * @date 2022-06-07
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "es8311.h"
#include <stddef.h>


#define POWER_ON_WAIT 	100

/**
 * Taken from ESP32 example, adapted and more self-explaining code
 * After this function, audio should be available by solely send through I2S
 */
bool ES8311_init(sampling_options_t sampling)
{
	uint8_t chip_read, aux;

	if (!ES8311_hardware_init(sampling))
	{
		return false;
	}
	/**
	 * NO OFICIAL SE CAMBIO SOLO PARA PROBAR
	 */
	es8311_delay(POWER_ON_WAIT);



	/**
	 * Chequeo de presencia del codec. Tiene que devolver si o si este valor, sino esta
	 * desconectado o roto
	 */
	if (  !ES8311_I2C_read(ES8311_CHIP_ID1, & chip_read ) ||
			chip_read != ES8311_DEFAULT_ID1 )
		return false;

	/* Reset ES8311 to its default */
	if(!ES8311_I2C_write(ES8311_RESET_REG00, RST_DIG | RST_CMG | RST_MST | RST_ADC_DIG | RST_DAC_DIG)) return false;	// 0x1F;

	es8311_delay(20);

	if(!ES8311_I2C_write(ES8311_RESET_REG00, 0x00)) return false;
	if(!ES8311_I2C_write(ES8311_RESET_REG00, CSM_ON)) return false;	//0x80  Power-on command

	/* Setup clock: source BCLK, polarities defaults, ADC and DAC clocks on */
	if(!ES8311_I2C_write(ES8311_CLK_MANAGER_REG01, MCLK_SEL | BCLK_ON | CLKADC_ON | CLKDAC_ON | ANACLKADC_ON | ANACLKDAC_ON)) return false;

	/* Frecuency config with BCLK = 32 * LRCK (automatic from uC frame) => IMCLK = 8 * BCLK
	 * Simple math ==> IMCLK = (32 * 8 * LRCK) , where LRCK = Sample Rate*/
	if(!ES8311_I2C_write(ES8311_CLK_MANAGER_REG02, MULT_PRE | DIV_PRE)) return false;


	/* ADC_FSMODE = 0 (single speed) and OSR = 32 --> OSR = IADC_CLK / LRCK / 8 */
	if(!ES8311_I2C_write(ES8311_CLK_MANAGER_REG03, 0x20)) return false;

	/* ADC_FSMODE = 0 (single speed) and OSR = 32 --> Just testing, not anywhere */
	if(!ES8311_I2C_write(ES8311_CLK_MANAGER_REG04, 0x20)) return false;

	/* ADC_SCALE = 0dB --> as User guide shows */
	if(!ES8311_I2C_read(ES8311_ADC_REG16, &aux)) return false;
	if(!ES8311_I2C_write(ES8311_ADC_REG16, aux & 0xF8)) return false;



	/* DIV_CLKADC=0 and DIV_CLKDAC=0 as defaults, but just for reasurement*/
	if(!ES8311_I2C_write(ES8311_CLK_MANAGER_REG05, DIV_CLKADC | DIV_CLKDAC)) return false;

	/* register ES8311_CLK_MANAGER_REG06 as default after POR - not meaningful in slave mode */
	/* register ES8311_CLK_MANAGER_REG07 as default after POR - not meaningful in slave mode */
	/* register ES8311_CLK_MANAGER_REG08 as default after POR - not meaningful in slave mode */

	/**
	 * REG06, REG07 and REG08 has no use in slave operation as DIV_BCLK and DIV_LRCK are disabled
	 */

	/* Setup audio format (fmt): master/slave, resolution, I2S
	 *
	 * Beware, example code has a new reset on REG00 with 0xBF value
	 * Not sure if needed here*/
//	if(!ES8311_I2C_write(ES8311_RESET_REG00, 0xBF);	//????????


	/* Setup SDP In and Out resolution 16bits both */
	if(!ES8311_I2C_write(ES8311_SDPIN_REG09, SDP_IN_WL_16BIT )) return false;
	if(!ES8311_I2C_write(ES8311_SDPOUT_REG0A, SDP_OUT_WL_16BIT)) return false;

	/**
	 * SDP_IN_FMT (REG09 [1:0]) have a default value (00) so its config as I2S serial audio data format
	 *
	 * Same with SDP_OUT_FMT
	 *
	 */

	if(!ES8311_I2C_write(ES8311_SYSTEM_REG0D, REG_0D_DEFAULT)) return false;	// Power up analog circuitry - NOT default
	if(!ES8311_I2C_write(ES8311_SYSTEM_REG0E, REG_0E_DEFAULT)) return false;	// Enable analog PGA, enable ADC modulator - NOT default
	if(!ES8311_I2C_write(ES8311_SYSTEM_REG12, REG_12_DEFAULT)) return false; 	// power-up DAC - NOT default
	if(!ES8311_I2C_write(ES8311_SYSTEM_REG13, HPSW)) return false;				// Enable output to HP drive - NOT default
	if(!ES8311_I2C_write(ES8311_ADC_REG1C, REG_1C_DEFAULT)) return false;		// ADC Equalizer bypass, cancel DC offset in digital domain

	/* DAC ramprate and bypass DAC equalizer - NOT default */
	if(!ES8311_I2C_write(ES8311_DAC_REG37, DAC_RAMPRATE_DEFAULT | DAC_EQBYPASS_DEFAULT)) return false;

	/* Set DAC Volume */
	if(!ES8311_I2C_write(ES8311_DAC_REG32, DAC_VOL_PERCENT_LEVEL(50) )) return false;

	/* Set ADC Volume */
	if(!ES8311_I2C_write(ES8311_ADC_REG17, ADC_VOL_PERCENT_LEVEL(50))) return false;

	/* Set max PGA Gain for Mic (differential input) and turn DIG_MIC off */
	if(!ES8311_I2C_write(ES8311_SYSTEM_REG14, LINSEL | PGAGAIN_15DB)) return false;

	return true;
}

void ES8311_deinit(void)
{
	ES8311_I2C_write(ES8311_RESET_REG00, RST_DIG | RST_CMG | RST_MST | RST_ADC_DIG | RST_DAC_DIG);
	ES8311_hardware_deinit();
}

bool ES8311_start(void)
{
	//return ES8311_I2S_start(buffer_Tx, buffer_Rx, BUFFER_LENGHT);
	return true;
}

void ES8311_mic_loop(uint16_t* tx, uint16_t* rx)
{
	 ES8311_I2S_loopStart(tx,rx);
}

bool ES8311_stop(void)
{
	return ES8311_I2S_stop();
}

bool ES8311_dac_level(int dac_level)
{
	/**
	 * TODO: Implement
	 */
	return true;

}

/**
 * @brief Map simple mic gain to I2C register
 * 
 *  MAX9860 Codec - Microphone Input Register - Address 0x0C
 * | b7 | b6  b5 | b4 b3 b2 b1 b0 |
 * | 0  | PREAMP | 	GAIN		  |
 * 
 * 0x00 to 0x19 -> disabled.
 * 0x20 to 0x3F -> 0 to 20 db
 * 0x40 to 0x5F -> 20 to 40 db
 * 0x60 to 0x7F -> 40 to 50 db
 * 
 * @param mic_gain between 0 and 50.
 * @return uint8_t I2C value register
 */

/**
 * TODO: IMPLEMENT
 */

bool ES8311_mic_gain(int mic_gain)
{

#if 0
	mic_input_t mic_input_register;

	if (mic_gain < 0 || mic_gain > MAX9860_MIC_GAIN_MAX)
	{
		return false;
	}

	/* Before to call this function, check volume is between 0 and 50 */
	if (mic_gain == 0)
	{
		mic_input_register.pam = MIC_PAM_DISABLED;
	}
	else if (mic_gain > 0 && mic_gain < 20)
	{
		mic_input_register.pam = MIC_PAM_0DB;
	}
	else if (mic_gain >= 20 && mic_gain < 30)
	{
		mic_input_register.pam = MIC_PAM_20DB;
		mic_gain -= 20;
	}
	else /* mic_gain > 30 && mic_gain <= 50 */
	{
		mic_input_register.pam = MIC_PAM_30DB;
		mic_gain -= 30;
	}

	/* Invert pgam because min gain is the max value */
	mic_gain = 20 - mic_gain;
	mic_input_register.pgam = mic_gain;

	max9860_I2C_write(MIC_GAIN, mic_input_register.value);
#endif
	return true;
}

/*
 * TODO: Check if necesary, only one mic on IC
 */
#if 0
bool ES8311_mic_select(codec_out_t codec_out)
{

	switch (codec_out)
	{
	case CODEC_OUT_MONO: //-> Both microphones.
	case CODEC_OUT_STEREO:
		max9860_I2C_write(ADC_LEVEL, ADC_LEVEL_LEFT_0DB | ADC_LEVEL_RIGHT_0DB);
		break;

	case CODEC_OUT_LEFT: //-> Electrect microphone.
		max9860_I2C_write(ADC_LEVEL, ADC_LEVEL_RIGHT_MUTE | ADC_LEVEL_LEFT_0DB);
		break;

	case CODEC_OUT_RIGHT: //-> Dynamic microphone.
		max9860_I2C_write(ADC_LEVEL, ADC_LEVEL_RIGHT_0DB | ADC_LEVEL_LEFT_MUTE);
		break;

	default:
		max9860_I2C_write(ADC_LEVEL, ADC_LEVEL_LEFT_MUTE | ADC_LEVEL_RIGHT_MUTE);
		return false;
		break;
	}

	return true;
}

/**
 * TODO: Implement
 */
bool ES8311_output_mix(uint8_t dac_gain, uint8_t adc_to_dac_level)
{

	output_mix_t output_mix;

	switch (dac_gain)
	{
	case DAC_GAIN_0DB:
	case DAC_GAIN_6DB:
	case DAC_GAIN_12DB:
	case DAC_GAIN_18DB:
		output_mix.dvg = dac_gain;
		break;

	default:
		return false;
		break;
	}

	if (adc_to_dac_level > 0x1F)
	{
		return false;
	}

	output_mix.dvst = adc_to_dac_level;

	max9860_I2C_write(OUTPUT_MIX, output_mix.value);

	return true;
}

/**
 * TODO: Implement if needed
 */
bool ES8311_HPF(bool enable_adc_filter, bool enable_dac_filter)
{

	uint8_t adc_filter;
	uint8_t dac_filter;

	switch (codec.sampling)
	{

	case SAMPLING_8K:
		adc_filter = (enable_adc_filter ? FILTER_ADC_ELLIPTICAL_8K : 0);
		dac_filter = (enable_dac_filter ? FILTER_DAC_ELLIPTICAL_8K : 0);
		break;

	case SAMPLING_16K:
		adc_filter = (enable_adc_filter ? FILTER_ADC_ELLIPTICAL_16K : 0);
		dac_filter = (enable_dac_filter ? FILTER_DAC_ELLIPTICAL_16K : 0);
		break;

	case SAMPLING_48K:
		adc_filter = (enable_adc_filter ? FILTER_ADC_BUTTERWORTH_48K : 0);
		dac_filter = (enable_dac_filter ? FILTER_DAC_BUTTERWORTH_48K : 0);
		break;

	default:
		return false; /* No available filters in another frequencies */
		break;
	}

	max9860_I2C_write(DIGITAL_FILTER, adc_filter | dac_filter);

	return true;
}
#endif
