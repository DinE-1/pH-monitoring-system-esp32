#include "adc.h"
#include "esp_adc/adc_continuous.h"
#include "esp_log.h"

#define ADC_CHANNEL ADC_CHANNEL_6
#define ADC_UNIT    ADC_UNIT_1

static const char *TAG = "ADC";

static adc_continuous_handle_t handle = NULL;

volatile uint16_t latest_sample = 0;
volatile uint32_t adc_samples = 0;
volatile uint16_t adc_min = 4095;
volatile uint16_t adc_max = 0;

void adc_dma_init_custom(void)
{
    adc_continuous_handle_cfg_t cfg = {
        .max_store_buf_size = 1024,
        .conv_frame_size = 128
    };
    adc_continuous_new_handle(&cfg, &handle);

    adc_continuous_config_t dig_cfg = {
        .sample_freq_hz = 20000,  // 20kHz
        .conv_mode      = ADC_CONV_SINGLE_UNIT_1,
        .format         = ADC_DIGI_OUTPUT_FORMAT_TYPE1
    };

    adc_digi_pattern_config_t pattern = {
        .atten = ADC_ATTEN_DB_12,
        .channel = ADC_CHANNEL,
        .unit = ADC_UNIT,
        .bit_width = ADC_BITWIDTH_12
    };

    dig_cfg.pattern_num = 1;
    dig_cfg.adc_pattern = &pattern;

    ESP_ERROR_CHECK(adc_continuous_config(handle, &dig_cfg));
    ESP_ERROR_CHECK(adc_continuous_start(handle));
}

void adc_dma_task(void *arg)
{
    uint8_t buffer[256];
    uint32_t length;

    while (1) {
        if (adc_continuous_read(handle, buffer, sizeof(buffer), &length, 1000) == ESP_OK) {
            adc_digi_output_data_t *data = (void*) buffer;
            int count = length / sizeof(adc_digi_output_data_t);

            for (int i = 0; i < count; i++) {
                uint16_t v = data[i].type1.data;
                latest_sample = v;
                adc_samples++;

                if (v < adc_min) adc_min = v;
                if (v > adc_max) adc_max = v;
            }
        }
    }
}

uint16_t adc_get_latest(void) { return latest_sample; }

void adc_get_stats(uint32_t *count, uint16_t *min, uint16_t *max)
{
    *count = adc_samples;
    *min = adc_min;
    *max = adc_max;

    adc_samples = 0;
    adc_min = 4095;
    adc_max = 0;
}

