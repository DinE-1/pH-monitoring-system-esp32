//for uint_type
#include <stdint.h>
//for NULL
#include <stddef.h>
//adc functions
#include "esp_adc/adc_continuous.h"

//initialise and configure the adc
#include "adc/adc_init_and_config.h"

void app_main(){
  //adc handle
  adc_continuous_handle_t adcHdl;
  //configure adc
  adc_init_and_config(&adcHdl);

  //start sampling adc
  adc_continuous_start(adcHdl);

  //adc results size
  uint8_t results[ADC_MEM_POOL_SIZE];
  //adc out length
  uint32_t out_length;
  //reading the results of the adc
  adc_continuous_read(adcHdl,results,ADC_MEM_POOL_SIZE,&out_length,adc_timeout_ms);
}
