#define ADC_MEM_POOL_SIZE 100

int main(){
  //adc cont mode driver configuration
  adc_continuous_handle_cfg_t adcHdlCfg={
    .max_store_buf_size=ADC_MEM_POOL_SIZE;
    .conv_frame_size=SOC_ADC_DIGI_DATA_BYTES_PER_CONV*2;
  };
  adc_continuous_handle_t adc=NULL;
  adc_continuous_new_handle(&adcHdlCfg,&adc);

  //adc channels config
  enum adc_channel_t chanNum=ADC_CHANNEL_3;
  adc_digi_pattern_config_t chCfgs[]={
    {
      .atten=ADC_ATTEN_DB_12;
      .channel=chanNum;
      .unit=1;
      .bit_width=12;
    }
  }
  //adc configuration
  adc_continuous_config_t adcCfg={
    .pattern_num=1;
    .adc_pattern=chCfgs;
    .sample_freq_hz=SOC_ADC_SAMPLE_FREQ_THRES_LOW;
    .conv_mode=ADC_CONV_SINGLE_UNIT_1;
    .format=ADC_DIGI_OUTPUT_FORMAT_TYPE1;
  }
}
