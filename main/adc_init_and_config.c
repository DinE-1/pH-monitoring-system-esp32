#include "adc/adc_init_and_config.h"
void adc_init_and_config(adc_continuous_handle_t *adc_handle){
  //adc handle
  adc_continuous_handle_cfg_t adcHdlCfg={
    .max_store_buf_size=ADC_MEM_POOL_SIZE,
    .conv_frame_size=SOC_ADC_DIGI_DATA_BYTES_PER_CONV*2
  };
  //get adc handle
  adc_continuous_new_handle(&adcHdlCfg,adc_handle);

  //adc channels
  adc_digi_pattern_config_t adcChnlsCfg[1]={
    {
      .atten=ADC_ATTEN_DB_12,
      .channel=ADC_CHANNEL_3,
      .unit=1,
      .bit_width=12,
    }
  };
  //adc configuration
  adc_continuous_config_t adcCfg;
  adcCfg=(adc_continuous_config_t){
    .pattern_num=1,
    .adc_pattern=adcChnlsCfg,
    .sample_freq_hz=SOC_ADC_SAMPLE_FREQ_THRES_LOW,
    .conv_mode=ADC_CONV_SINGLE_UNIT_1,
    .format=ADC_DIGI_OUTPUT_FORMAT_TYPE1,
  };

  //configure adc
  adc_continuous_config(*adc_handle,&adcCfg);
}
