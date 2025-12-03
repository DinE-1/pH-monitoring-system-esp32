#ifndef ADC_INIT_AND_CONFIG_H
#define ADC_INIT_AND_CONFIG_H

//for uint_type
#include <stdint.h>
//for NULL
#include <stddef.h>
//adc functions
#include "esp_adc/adc_continuous.h"

//total memory size for adc readings
#define ADC_MEM_POOL_SIZE 100
//adc read timeout
#define adc_timeout_ms 5000

//adc handle
adc_continuous_handle_t adcHdl;
//adc results size
uint8_t results[ADC_MEM_POOL_SIZE];
//adc out length
uint32_t out_length;

//initialise and configure adc
void adc_init_and_config(adc_continuous_handle_t *adc_handle);

//adc handle
adc_continuous_handle_cfg_t adcHdlCfg;
//adc channels
adc_digi_pattern_config_t adcChnlsCfg[1];
//adc configuration
adc_continuous_config_t adcCfg;

#endif
