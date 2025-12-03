//adc functions
#include "esp_adc/adc_continuous.h"

//initialise and configure the adc
#include "adc/adc_init_and_config.h"

void adc_setup(){
  //configure adc
  adc_init_and_config(&adcHdl);
  //start adc
  adc_continuous_start(adcHdl);
}

void app_main(){
  //setup adc
  adc_setup();
  //reading the results of the adc
  adc_continuous_read(adcHdl,results,ADC_MEM_POOL_SIZE,&out_length,adc_timeout_ms);
}
