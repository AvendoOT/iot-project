#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_log.h"
#include "protocol_examples_common.h"
#include "mqtt.h"
#include "808H5V5.h"
#include "mcp9700.h"

#define CHANNEL1 ADC_CHANNEL_6

static const char *TAG_MAIN = "ultrax-softAP";

extern "C" void app_main(void)
{
    ESP_LOGI(TAG_MAIN, "[APP] Free memory: %d bytes", esp_get_free_heap_size());
    ESP_LOGI(TAG_MAIN, "[APP] IDF version: %s", esp_get_idf_version());

    esp_log_level_set("*", ESP_LOG_INFO);
    esp_log_level_set("esp-tls", ESP_LOG_VERBOSE);
    esp_log_level_set("MQTT_CLIENT", ESP_LOG_VERBOSE);
    esp_log_level_set("MQTT_EXAMPLE", ESP_LOG_VERBOSE);
    esp_log_level_set("TRANSPORT_BASE", ESP_LOG_VERBOSE);
    esp_log_level_set("TRANSPORT", ESP_LOG_VERBOSE);
    esp_log_level_set("OUTBOX", ESP_LOG_VERBOSE);

    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    ESP_ERROR_CHECK(example_connect());

    mqtt_app_start();

    // MCP9700A *mcpPtr = new MCP9700A(CHANNEL1);
    // H808H5V5 *h808Ptr = new H808H5V5(CHANNEL1);

    // mcpPtr->readTemp();
    // printf("mcpPtr->getTemp()\n");
    // printf("mcpPtr->getTemp(): %f\n", mcpPtr->getTemp());

    while (1)
    {
        // int value = adc1_get_voltage(channel);
        // ESP_LOGI(tag, "value %d", value);
        vTaskDelay(pdMS_TO_TICKS(1000));
        // mcpPtr->readTemp();
        // printf("mcpPtr->getTemp(): %f\n", mcpPtr->getTemp());
    }
}