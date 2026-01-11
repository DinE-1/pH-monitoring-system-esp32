#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "adc.h"
#include "wifi.h"
#include "webserver.h"

static const char* TAG = "MAIN";

void monitor_task(void *arg)
{
    while (1) {
        uint32_t count;
        uint16_t min, max, latest;
        adc_get_stats(&count, &min, &max);
        latest = adc_get_latest();

        ESP_LOGI("SCOPE", "Samples/s=%lu Min=%u Max=%u Latest=%u", count, min, max, latest);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void app_main(void)
{
    ESP_LOGI(TAG, "Initializing Wi-Fi...");
    wifi_init();
    while (!wifi_is_connected()) {
        vTaskDelay(pdMS_TO_TICKS(500));
    }
    ESP_LOGI(TAG, "Wi-Fi connected");

    ESP_LOGI(TAG, "Starting web server...");
    webserver_start();

    ESP_LOGI(TAG, "Initializing ADC...");
    adc_dma_init_custom();
    xTaskCreate(adc_dma_task, "adc_task", 2048, NULL, 5, NULL);

    ESP_LOGI(TAG, "Starting monitor task...");
    xTaskCreate(monitor_task, "monitor", 2048, NULL, 1, NULL);
}

