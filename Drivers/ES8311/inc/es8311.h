/**
 * @file es8311.h
 * @author Gonzalo E. Sanchez (gonzalo.e.sds@gmail.com)
 * @brief es8311 API.
 * @version 0.1
 * @date 2021-06-04
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#ifndef ES8311_H
#define ES8311_H


#include <stdbool.h>
#include <stdint.h>

#include "main.h"
#define es8311_delay(x) HAL_Delay(x)
#define CONFIG_USE_ES8311_A0_HIGH 0

#define BUFFER_LENGHT	2048
/**
 * Esta configuracion depende del pin correspondiente. El circuito basico trae el pin A0 --> GND
 * por lo que se usa la direccion 0x18 mayormente
 */
#if CONFIG_USE_ES8311_A0_HIGH
#define ES8311_I2C_ADDR 0x19
#else
#define ES8311_I2C_ADDR 0x18
#endif


/******************************************************************************
 * 				DEFINICIONES DE REGISTROS PARA ES8311
 *****************************************************************************/
#define ES8311_RESET_REG00			0x00  /*reset digital,csm,clock manager etc.*/
#define ES8311_CLK_MANAGER_REG01	0x01 /* select clk src for mclk, enable clock for codec */
#define ES8311_CLK_MANAGER_REG02	0x02 /* clk divider and clk multiplier */
#define ES8311_CLK_MANAGER_REG03	0x03 /* adc fsmode and osr  */
#define ES8311_CLK_MANAGER_REG04	0x04 /* dac osr */
#define ES8311_CLK_MANAGER_REG05	0x05 /* clk divier for adc and dac */
#define ES8311_CLK_MANAGER_REG06	0x06 /* bclk inverter and divider */
#define ES8311_CLK_MANAGER_REG07	0x07 /* tri-state, lrck divider */
#define ES8311_CLK_MANAGER_REG08	0x08 /* lrck divider */
#define ES8311_SDPIN_REG09			0x09 /* dac serial digital port */
#define ES8311_SDPOUT_REG0A			0x0A /* adc serial digital port */
#define ES8311_SYSTEM_REG0B			0x0B /* system */
#define ES8311_SYSTEM_REG0C			0x0C /* system */
#define ES8311_SYSTEM_REG0D			0x0D /* system, power up/down */
#define ES8311_SYSTEM_REG0E			0x0E /* system, power up/down */
#define ES8311_SYSTEM_REG0F			0x0F /* system, low power */
#define ES8311_SYSTEM_REG10			0x10 /* system */
#define ES8311_SYSTEM_REG12			0x12 /* system, Enable DAC */
#define ES8311_SYSTEM_REG13			0x13 /* system */
#define ES8311_SYSTEM_REG14			0x14 /* system, select DMIC, select analog pga gain */
#define ES8311_ADC_REG15			0x15 /* ADC, adc ramp rate, dmic sense */
#define ES8311_ADC_REG16			0x16 /* ADC */
#define ES8311_ADC_REG17			0x17 /* ADC, volume */
#define ES8311_ADC_REG18			0x18 /* ADC, alc enable and winsize */
#define ES8311_ADC_REG19			0x19 /* ADC, alc maxlevel */
#define ES8311_ADC_REG1A			0x1A /* ADC, alc automute */
#define ES8311_ADC_REG1B			0x1B /* ADC, alc automute, adc hpf s1 */
#define ES8311_ADC_REG1C			0x1C /* ADC, equalizer, hpf s2 */
#define ES8311_DAC_REG31			0x31 /* DAC, mute */
#define ES8311_DAC_REG32			0x32 /* DAC, volume */
#define ES8311_DAC_REG33			0x33 /* DAC, offset */
#define ES8311_DAC_REG34			0x34 /* DAC, drc enable, drc winsize */
#define ES8311_DAC_REG35			0x35 /* DAC, drc maxlevel, minilevel */
#define ES8311_DAC_REG37			0x37 /* DAC, ramprate */
#define ES8311_GPIO_REG44			0x44 /* GPIO, dac2adc for test */
#define ES8311_GP_REG45				0x45 /* GP CONTROL */
#define ES8311_CHIP_ID1				0xFD /* CHIP ID1 */
#define ES8311_CHIP_ID2				0xFE /* CHIP ID2 */
#define ES8311_CHIP_VER				0xFF /* VERSION */
#define ES8311_MAX_REGISTER			0xFF

#define ES8311_DEFAULT_ID1			0x83
#define ES8311_DEFAULT_ID2			0x11
#define ES8311_DEFAULT_VER			0x00


/**
 * Los registros no listados estan aqui especificados (segun "ESP8311 DS.pdf")
 *
 * ES8311_SYSTEM_REG11		0x11 --> system - internal use, not to be modified
 *
 */

/******************************************************************************
 * 				DEFINICIONES DE BITS PARA ES8311
 *****************************************************************************/

//------------------------- RESET REG 0x00 -----------------------------------

#define CSM_ON			0x80		//<--- Mask of CSM_ON bit
#define MSC				0x40		//<--- Mask of MSC bit
#define SEQ_DIS			0x20		//<--- Mask of SEQ_DIS bit
#define RST_DIG			0x10		//<--- Mask of RST_DIG bit
#define RST_CMG			0x08		//<--- Mask of RST_CMG bit
#define RST_MST			0x04		//<--- Mask of RST_MST bit
#define RST_ADC_DIG		0x02		//<--- Mask of RST_ADC_DIG bit
#define RST_DAC_DIG		0x01		//<--- Mask of RST_DAC_DIG bit

//------------------------- CLK MANAGER REG 0x01 -----------------------------

#define MCLK_SEL		0x80		//<--- Mask of MCLK_SEL bit
#define MCLK_INV		0x40		//<--- Mask of MCLK_INV bit
#define MCLK_ON			0x20		//<--- Mask of MCLK_ON bit
#define BCLK_ON			0x10		//<--- Mask of BCLK_ON bit
#define CLKADC_ON		0x08		//<--- Mask of CLKADC_ON bit
#define CLKDAC_ON		0x04		//<--- Mask of CLKDAC_ON bit
#define ANACLKADC_ON	0x02		//<--- Mask of ANACLKADC_ON bit
#define ANACLKDAC_ON	0x01		//<--- Mask of ANACLKDAC_ON bit

//------------------------- CLK MANAGER REG 0x06 -----------------------------

#define BCLK_CON			0x40	//<--- Mask of BCLK_CON bit
#define BCLK_INV			0x20	//<--- Mask of BCLK_INV bit

//Valid values for BCLK divider at Master mode DIV_BCLK[4:0]

#define DIV_BCLK_DEFAULT	0x03	//<--- Default value MCLK/4
#define DIV_BCLK_MCLK22		0x14	//<--- MCLK/22
#define DIV_BCLK_MCLK24		0x15	//<--- MCLK/24
#define DIV_BCLK_MCLK25		0x16	//<--- MCLK/25
#define DIV_BCLK_MCLK30		0x17	//<--- MCLK/30
#define DIV_BCLK_MCLK32		0x18	//<--- MCLK/32
#define DIV_BCLK_MCLK33		0x19	//<--- MCLK/33
#define DIV_BCLK_MCLK34		0x1A	//<--- MCLK/34
#define DIV_BCLK_MCLK36		0x1B	//<--- MCLK/36
#define DIV_BCLK_MCLK44		0x1C	//<--- MCLK/44
#define DIV_BCLK_MCLK48		0x1D	//<--- MCLK/48
#define DIV_BCLK_MCLK66		0x1E	//<--- MCLK/66
#define DIV_BCLK_MCLK72		0x1F	//<--- MCLK/72

//------------------------- SDP REG 0x09 -------------------------------------
/**
 * NOTA: El software esta preparado para resolucion de 16 bits, asi que esa
 * configuracion se hace hardcoded.
 */

#define SDP_IN_MUTE			0x40		//<--- Mask of SDP_IN_MUTE bit
#define SDP_IN_WL_16BIT		0x0C		//<--- Value IN Word Length 16 bits
#define SDP_IN_FMT_LEFT		0x01		//<--- Value for left justify serial audio data format

//------------------------- SDP REG 0x0A -------------------------------------
/**
 * NOTA: El software esta preparado para resolucion de 16 bits, asi que esa
 * configuracion se hace hardcoded.
 */

#define SDP_OUT_MUTE			0x40		//<--- Mask of SDP_OUT_MUTE bit
#define SDP_OUT_WL_16BIT		0x0C		//<--- Value OUT Word Length 16 bits
#define SDP_OUT_FMT_LEFT		0x01		//<--- Value for left justify serial audio data format

//------------------------- SYSTEM REG 0x0D ----------------------------------

#define REG_0D_DEFAULT			0x01		//<--- Value Start Up normal for automatic startup

//------------------------- SYSTEM REG 0x0E ----------------------------------

#define REG_0E_DEFAULT			0x02		//<--- Value for default startup

//------------------------- SYSTEM REG 0x12 ----------------------------------

#define REG_12_DEFAULT			0x00		//<--- Value for default startup

//------------------------- SYSTEM REG 0x13 ----------------------------------

#define HPSW					0x10		//<--- Mask of HPSW bit

//------------------------- SYSTEM REG 0x14 ----------------------------------

#define REG_14_DEFAULT			0x1A		//<---- Value for max PGA gain and DMIC off

#define LINSEL					0x10		//<---- Value for Mic1p-1n select

enum PGAGAIN {
								PGAGAIN_0DB = 0,	//<--- Values for several MIC PGA gains
								PGAGAIN_3DB,
								PGAGAIN_6DB,
								PGAGAIN_9DB,
								PGAGAIN_12DB,
								PGAGAIN_15DB,
								PGAGAIN_18DB,
								PGAGAIN_21DB,
								PGAGAIN_24DB,
								PGAGAIN_27DB,
								PGAGAIN_30DB,
};

//------------------------- ADC REG 0x1C -------------------------------------

#define REG_1C_DEFAULT			0x6A		//<--- Value for default startup

//------------------------	DAC VOL REG 0x32 ---------------------------------

#define DAC_VOLUME_0DB			0xBF		//<--- 0 dB for DAC volume (not default)

//------------------------- DAC REG 0x37 -------------------------------------

#define DAC_RAMPRATE_DEFAULT	0x40		//<--- Value for 0.25dB/32LRCK
#define DAC_EQBYPASS_DEFAULT	0x08		//<--- Value for DAC_EQ bypass





/******************************************************************************
 * 		DEFINICIONES ESPECIFICAS PARA UTILIZAR BCLK EN VEZ DE MCLK
 *****************************************************************************/
/* INTERNAL MASTER CLOCK = 8 * BCLK */

#define MULT_PRE	0x18		//<--- Value mask for MULT_PRE = 8 bits [4:3]
#define DIV_PRE		0x00		//<--- Value mask for DIV_PRE = 0 bits [7:5]
#define DIV_CLKADC	0x00		//<--- Value mask for DIV_CLKADC = 0 bits [7:4]
#define DIV_CLKDAC	0x00		//<--- Value mask for DIV_CLKDAC = 0 bits [3:0]



/******************************************************************************
 * 								HELPER FUNCTIONS
 *****************************************************************************/
/**
 * @brief This is a translation of volume levels from percentage to uint8_t
 *
 * The volume of DAC goes in steps of 0.5dB from -95.5dB to +32dB
 * This function translates a percentage to a decimal value of one byte
 * corresponding to a level as Datasheets shows
 */
#define DAC_VOL_PERCENT_LEVEL(x)	((255 * x) / 100)

#define ADC_VOL_PERCENT_LEVEL(x)	DAC_VOL_PERCENT_LEVEL(x)

/******************************************************************************
 * 							DEFINICIONES VARIAS
 *****************************************************************************/


typedef enum sampling_options
{
	SAMPLING_8K, SAMPLING_11K, SAMPLING_16K, SAMPLING_22K
} sampling_options_t;


/******************************************************************************
 * 				PROTOTIPO DE FUNCIONES PARA ES8311
 *****************************************************************************/

/**
 * @brief Hardware init for es8311
 * Configure I2C, I2S and clocks
 *
 */
bool ES8311_hardware_init(sampling_options_t sampling);

/**
 * @brief Release configuration of clocks.
 * 
 */
void ES8311_hardware_deinit(void);

/**
 * @brief Write one value in one I2C register of ES8311.
 * 
 * @param reg 
 * @param value 
 */
bool ES8311_I2C_write(const uint8_t reg, uint8_t value);

/**
 * @brief Read one value in one I2C register of ES8311.
 * 
 * @param reg 
 * @return uint8_t 
 */
bool ES8311_I2C_read(const uint8_t reg , uint8_t * out);

/**
 * @brief Start I2S DMA transmit and receive
 * 
 * @return true if successful
 * @return false hardware error.
 */
bool ES8311_I2S_start(int16_t *buffer_tx, int16_t *buffer_rx, uint16_t buffer_length);

void ES8311_I2S_loopStart(uint16_t* tx, uint16_t* rx);

/**
 * @brief Stop I2S DMA transmit and receive
 * 
 * @return true if successful
 * @return false hardware error.
 */
bool ES8311_I2S_stop(void);

/**
 * @brief
 *
 */
bool ES8311_init(sampling_options_t sampling);

/**
 * @brief
 *
 */
void ES8311_deinit(void);

/**
 * @brief
 *
 */
bool ES8311_start(void);

void ES8311_mic_loop(uint16_t* tx, uint16_t* rx);

/**
 * @brief
 *
 */
bool ES8311_stop(void);

/**
 * @brief
 *
 */
bool ES8311_dac_level(int dac_level);

/**
 * @brief
 *
 */
//bool ES8311_mic_select(codec_out_t codec_out);

/**
 * @brief
 *
 */
bool ES8311_output_mix(uint8_t dac_gain, uint8_t adc_to_dac_level);

/**
 * @brief
 *
 */
bool ES8311_mic_gain(int mic_gain);

/**
 * @brief
 *
 */
bool ES8311_HPF(bool enable_adc_filter, bool enable_dac_filter);


#endif /* ES8311_H */
