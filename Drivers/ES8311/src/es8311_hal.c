/**
 * @file es8311_hal.c
 * @author Gonzalo E. Sanchez (gonzalo.e.sds@gmail.com)
 * @brief ES8311 HAL DRIVER
 * @version 0.1
 * @date 2021-06-04
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include "es8311.h"
#include "main.h"
#include <string.h>

#define CONFIG_CODEC_ES8311 1

#if CONFIG_CODEC_ES8311

/**
 * Crear un core correspondiente a IPAC19 v2.1
 * Se utiliza la instancia de I2S2 en vez de la I2S3 que en su momento usaba la Wanpage
 */

extern I2C_HandleTypeDef hi2c2;

extern I2S_HandleTypeDef hi2s2;

#define I2S_HAL_HANDLER &hi2s2                    //!< This depends on number of peripheral use in STM32
#define I2C_HAL_HANDLER &hi2c2                    //!< This depends on number of peripheral use in STM32
#define I2C_TIMEOUT 50                            //!< Milliseconds

bool ES8311_hardware_init(sampling_options_t sampling)
{
    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

    /*	Adjust the audio frequency. */
    switch (sampling)
    {
    case SAMPLING_8K:
        PeriphClkInitStruct.PLLI2S.PLLI2SN = 128;
        PeriphClkInitStruct.PLLI2S.PLLI2SR = 2;
        hi2s2.Init.AudioFreq = I2S_AUDIOFREQ_8K;
        break;

    case SAMPLING_11K:
        PeriphClkInitStruct.PLLI2S.PLLI2SN = 350;
        PeriphClkInitStruct.PLLI2S.PLLI2SR = 4;
        hi2s2.Init.AudioFreq = I2S_AUDIOFREQ_11K;
        break;

    case SAMPLING_16K:
        PeriphClkInitStruct.PLLI2S.PLLI2SN = 256;
        PeriphClkInitStruct.PLLI2S.PLLI2SR = 2;
        hi2s2.Init.AudioFreq = I2S_AUDIOFREQ_16K;
        break;

    case SAMPLING_22K: /* Real frequency = 22.051 KHz */
        PeriphClkInitStruct.PLLI2S.PLLI2SN = 350;
        PeriphClkInitStruct.PLLI2S.PLLI2SR = 4;
        hi2s2.Init.AudioFreq = I2S_AUDIOFREQ_22K;
        break;

    default:
        return false; /* Not a valid Frequency */
        break;
    }

    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_I2S;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
      return false;
    }
/*
    hi2s2.Instance = SPI2;
    hi2s2.Init.Mode = I2S_MODE_MASTER_TX;
    hi2s2.Init.Standard = I2S_STANDARD_PHILIPS;
    hi2s2.Init.DataFormat = I2S_DATAFORMAT_16B;
    hi2s2.Init.MCLKOutput = I2S_MCLKOUTPUT_DISABLE;
    hi2s2.Init.CPOL = I2S_CPOL_HIGH;
    hi2s2.Init.ClockSource = I2S_CLOCK_PLL;
    hi2s2.Init.FullDuplexMode = I2S_FULLDUPLEXMODE_ENABLE;
*/
    if (HAL_I2S_Init(I2S_HAL_HANDLER) != HAL_OK)
    {
        return false;
    }
    return true;
}

void ES8311_hardware_deinit(void)
{
    HAL_I2S_DMAStop(I2S_HAL_HANDLER);
    HAL_I2S_DeInit(I2S_HAL_HANDLER);
}

bool ES8311_I2C_write(const uint8_t reg, uint8_t value)
{
	HAL_StatusTypeDef status;
	bool ret = false;

	status = HAL_I2C_Mem_Write(I2C_HAL_HANDLER, ES8311_I2C_ADDR << 1, reg, sizeof(reg), &value, sizeof(value), I2C_TIMEOUT);

	if (status == HAL_OK)
		ret = true;

	return ret;
}

bool ES8311_I2C_read(const uint8_t reg, uint8_t * out )
{
	HAL_StatusTypeDef status;
	bool ret = false;

	status =  HAL_I2C_Mem_Read(I2C_HAL_HANDLER, ES8311_I2C_ADDR << 1, reg, sizeof(reg), out, sizeof(uint8_t), I2C_TIMEOUT);

	if (status == HAL_OK)
		ret = true;

	return ret;
}


void ES8311_I2S_loopStart (uint16_t* tx, uint16_t* rx)  {
	volatile HAL_StatusTypeDef ret;
	uint16_t *aux_pointer;

	while(1)  {

		ret = HAL_I2SEx_TransmitReceive(I2S_HAL_HANDLER, tx, rx, BUFFER_LENGHT, 10);

		aux_pointer = tx;
		tx = rx;
		rx = aux_pointer;
		//HAL_Delay(2);
	}
}


bool ES8311_I2S_start(int16_t *buff_tx, int16_t *buff_rx, uint16_t buff_length)
{
    /* Try start audio tranfer 3 times */
    uint8_t tries = 3;

    while (tries--)
    {
        if (HAL_OK == HAL_I2SEx_TransmitReceive_DMA(I2S_HAL_HANDLER,
                                                    (uint16_t *)buff_tx,
                                                    (uint16_t *)buff_rx,
                                                    buff_length))
        {
            return true;
        }

        HAL_Delay(100);
    }

    return false;
}



bool ES8311_I2S_stop(void)
{
    if (HAL_OK == HAL_I2S_DMAStop(I2S_HAL_HANDLER))
    {
        return true;
    }
    else
    {
        return false;
    }
}
#if 0
//!< Half TX-RX buffer interrupt
void
HAL_I2SEx_TxRxHalfCpltCallback(I2S_HandleTypeDef *hi2s)
{

}


//!< Complete TX-RX buffer interrupt

void
HAL_I2SEx_TxRxCpltCallback ( I2S_HandleTypeDef * hi2s )
{
	uint16_t *aux_pointer;
	// El MIC siempre contribuye datos

	/**
	 * Se hace un estilo ping pong
	 */
	aux_pointer = buffer_Tx;
	buffer_Tx = buffer_Rx;
	buffer_Rx = aux_pointer;

    HAL_I2SEx_TransmitReceive_DMA (
		I2S_HAL_HANDLER,
        (uint16_t *) codec.buffer_tx,
        (uint16_t *) codec.buffer_rx,
        codec.buffer_length
	);
}

void HAL_I2S_ErrorCallback(I2S_HandleTypeDef *hi2s)
{
    HAL_I2SEx_TransmitReceive_DMA(I2S_HAL_HANDLER,
                                  (uint16_t *)codec.buffer_tx,
                                  (uint16_t *)codec.buffer_rx,
                                  codec.buffer_length);
}
#endif
#endif /* IPAC_PLATFORM == WANPAGE_9 */
