#ifndef ADC_INIT_AND_CONFIG_H
#define ADC_INIT_AND_CONFIG_H

//adc datatype definitions
#include "esp_adc/adc_continuous.h"

//total memory size for adc readings
#define ADC_MEM_POOL_SIZE 100
//adc read timeout
#define adc_timeout_ms 5000

//initialise and configure adc
void adc_init_and_config(adc_continuous_handle_t *adc_handle);

#endif
