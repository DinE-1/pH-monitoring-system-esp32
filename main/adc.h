#pragma once
#include <stdint.h>

void adc_dma_init_custom(void);
void adc_dma_task(void *arg);
uint16_t adc_get_latest(void);
void adc_get_stats(uint32_t *count, uint16_t *min, uint16_t *max);

