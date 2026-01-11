#include "webserver.h"
#include "esp_http_server.h"
#include "esp_log.h"
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "adc.h"

static const char *TAG = "WEB";

httpd_handle_t server = NULL;
int ws_client_fd = -1;

static void ws_push_task(void *arg)
{
    while (1) {
        if (ws_client_fd >= 0 && server) {
            char msg[64];
            uint16_t value = adc_get_latest();
            float voltage = value * 3.3f / 4095;
            snprintf(msg, sizeof(msg), "{\"voltage\":%.3f}", voltage);

            httpd_ws_frame_t ws_pkt = {
                .type = HTTPD_WS_TYPE_TEXT,
                .payload = (uint8_t*)msg,
                .len = strlen(msg)
            };
            esp_err_t ret = httpd_ws_send_frame_async(server, ws_client_fd, &ws_pkt);
            if (ret != ESP_OK) ws_client_fd = -1;
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

static esp_err_t index_handler(httpd_req_t *req)
{
    FILE *f = fopen("/spiffs/index.html", "r");
    if (!f) {
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "index.html missing");
        return ESP_FAIL;
    }
    char buf[256];
    size_t len;
    httpd_resp_set_type(req, "text/html");
    while ((len = fread(buf, 1, sizeof(buf), f)) > 0)
        httpd_resp_send_chunk(req, buf, len);
    fclose(f);
    httpd_resp_send_chunk(req, NULL, 0);
    return ESP_OK;
}

static esp_err_t ws_handler(httpd_req_t *req)
{
    if (req->method == HTTP_GET) {
        ws_client_fd = httpd_req_to_sockfd(req);
        ESP_LOGI(TAG, "WS connected fd=%d", ws_client_fd);
        static bool task_started = false;
        if (!task_started) {
            xTaskCreate(ws_push_task, "ws_push_task", 4096, NULL, 5, NULL);
            task_started = true;
        }
        return ESP_OK;
    }
    return ESP_OK;
}

void webserver_start(void)
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.server_port = 8080;

    if (httpd_start(&server, &config) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to start HTTP server");
        return;
    }

    ESP_LOGI(TAG, "Webserver started on port %d", config.server_port);

    httpd_uri_t root = {
        .uri = "/",
        .method = HTTP_GET,
        .handler = index_handler
    };
    httpd_register_uri_handler(server, &root);

    httpd_uri_t ws_uri = {
        .uri = "/ws",
        .method = HTTP_GET,
        .handler = ws_handler,
        .is_websocket = true
    };
    httpd_register_uri_handler(server, &ws_uri);
}

