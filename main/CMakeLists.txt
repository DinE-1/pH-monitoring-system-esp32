idf_component_register(
    SRCS "main.c" "wifi.c" "adc.c" "webserver.c"
    INCLUDE_DIRS "."
    REQUIRES "esp_wifi" "esp_netif" "nvs_flash" "esp_http_server" "esp_adc" "spiffs" "freertos" "esp_timer"
)

