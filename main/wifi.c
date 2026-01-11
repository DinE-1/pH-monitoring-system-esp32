#include "wifi.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "nvs_flash.h"
#include "esp_spiffs.h"
#include <string.h>
#include <stdio.h>

static const char *TAG = "WIFI";

#define MAX_SSID_LEN 32
#define MAX_PASS_LEN 64

static void mount_spiffs(void)
{
    esp_vfs_spiffs_conf_t conf = {
        .base_path = "/spiffs",
        .partition_label = "spiffs",
        .max_files = 5,
        .format_if_mount_failed = true
    };
    esp_err_t ret = esp_vfs_spiffs_register(&conf);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to mount SPIFFS: %d", ret);
    } else {
        ESP_LOGI(TAG, "SPIFFS mounted at /spiffs");
    }
}

static bool read_wifi_credentials(char *ssid, char *password)
{
    FILE *f = fopen("/spiffs/credentials.txt", "r");
    if (!f) {
        ESP_LOGW(TAG, "credentials.txt not found, using defaults");
        return false;
    }

    if (fgets(ssid, MAX_SSID_LEN, f) == NULL) { fclose(f); return false; }
    ssid[strcspn(ssid, "\r\n")] = 0;

    if (fgets(password, MAX_PASS_LEN, f) == NULL) { fclose(f); return false; }
    password[strcspn(password, "\r\n")] = 0;

    fclose(f);
    ESP_LOGI(TAG, "Read Wi-Fi credentials: SSID='%s'", ssid);
    return true;
}

static const char* reason_to_string(wifi_err_reason_t reason)
{
    switch(reason) {
        case WIFI_REASON_UNSPECIFIED: return "Unspecified";
        case WIFI_REASON_AUTH_EXPIRE: return "Auth expired";
        case WIFI_REASON_AUTH_FAIL:   return "Auth failed (wrong password?)";
        case WIFI_REASON_NO_AP_FOUND: return "AP not found";
        case WIFI_REASON_ASSOC_FAIL:  return "Association failed";
        default: return "Other";
    }
}

// Wi-Fi event handler
static void wifi_event_handler(void* arg, esp_event_base_t event_base,
                               int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        ESP_LOGI(TAG, "STA started, attempting to connect...");
        esp_wifi_connect();
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        wifi_event_sta_disconnected_t* disconn = (wifi_event_sta_disconnected_t*) event_data;
        ESP_LOGW(TAG, "Failed to connect to SSID: %.*s, reason: %s (%d)",
                 disconn->ssid_len, disconn->ssid,
                 reason_to_string(disconn->reason),
                 disconn->reason);

        esp_wifi_connect();
    }
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        char ip_str[16];
        esp_ip4addr_ntoa(&event->ip_info.ip, ip_str, sizeof(ip_str));
        ESP_LOGI(TAG, "Connected successfully, IP: %s", ip_str);
    }
}

void wifi_init(void)
{
    ESP_LOGI(TAG, "Initializing NVS...");
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_LOGI(TAG, "Initializing TCP/IP stack...");
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    static esp_event_handler_instance_t instance_any_id;
    static esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID,
                                                       &wifi_event_handler, NULL, &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP,
                                                       &wifi_event_handler, NULL, &instance_got_ip));

    mount_spiffs();

    char ssid[MAX_SSID_LEN] = {0};
    char password[MAX_PASS_LEN] = {0};
    if (!read_wifi_credentials(ssid, password)) {
        strcpy(ssid, "default_ssid");
        strcpy(password, "default_pass");
    }

    wifi_config_t wifi_config = {0};
    strncpy((char*)wifi_config.sta.ssid, ssid, sizeof(wifi_config.sta.ssid));
    strncpy((char*)wifi_config.sta.password, password, sizeof(wifi_config.sta.password));

    ESP_LOGI(TAG, "Setting Wi-Fi mode to STA and starting...");
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
}

bool wifi_is_connected(void)
{
    wifi_ap_record_t ap_info;
    if (esp_wifi_sta_get_ap_info(&ap_info) == ESP_OK) {
        ESP_LOGI(TAG, "Wi-Fi connected to SSID: %s", ap_info.ssid);
        return true;
    }
    return false;
}

