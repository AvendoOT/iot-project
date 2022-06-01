#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_log.h"
#include "protocol_examples_common.h"
#include "mqtt_client.h"
#include "esp_system.h"
#include "esp_tls.h"
#include "esp_ota_ops.h"
#include <sys/param.h>
#include "led_strip.h"
#include "808H5V5.h"
#include "mcp9700.h"
#include "CLed.h"

#define CHANNEL1 ADC_CHANNEL_6
#define LED_PIN 8
#define ACTIVATE_TOPIC "activate"

static const char *TAG_MAIN = "iot-project";

bool isLed = false;

extern const uint8_t cert_start[] asm("_binary_AmazonRootCA1_pem_start");
extern const uint8_t cert_end[] asm("_binary_AmazonRootCA1_pem_end");
extern const uint8_t certificate_start[] asm("_binary_certificate_pem_crt_start");
extern const uint8_t certificate_end[] asm("_binary_certificate_pem_crt_end");
extern const uint8_t private_start[] asm("_binary_private_pem_key_start");
extern const uint8_t private_end[] asm("_binary_private_pem_key_end");

esp_mqtt_client_handle_t client;

void send_binary(esp_mqtt_client_handle_t client)
{
    spi_flash_mmap_handle_t out_handle;
    const void *binary_address;
    const esp_partition_t *partition = esp_ota_get_running_partition();
    esp_partition_mmap(partition, 0, partition->size, SPI_FLASH_MMAP_DATA, &binary_address, &out_handle);
    // sending only the configured portion of the partition (if it's less than the partition size)
    int binary_size = MIN(CONFIG_BROKER_BIN_SIZE_TO_SEND, partition->size);
    int msg_id = esp_mqtt_client_publish(client, "/topic/binary", (const char *)binary_address, binary_size, 0, 0);
    ESP_LOGI(TAG_MAIN, "binary sent with msg_id=%d", msg_id);
}

void getData(esp_mqtt_event_handle_t event)
{
    if (event)
    {
        isLed = !isLed;
    }
    printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
    printf("DATA=%.*s\r\n", event->data_len, event->data);
}

extern "C" void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    ESP_LOGD(TAG_MAIN, "Event dispatched from event loop base=%s, event_id=%d", base, event_id);
    esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t)event_data;
    client = (esp_mqtt_client_handle_t)event->client;
    int msg_id;
    switch ((esp_mqtt_event_id_t)event_id)
    {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG_MAIN, "MQTT_EVENT_CONNECTED");
        msg_id = esp_mqtt_client_subscribe(client, ACTIVATE_TOPIC, 0);
        ESP_LOGI(TAG_MAIN, "sent subscribe successful, msg_id=%d", msg_id);
        break;
    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(TAG_MAIN, "MQTT_EVENT_DISCONNECTED");
        break;
    case MQTT_EVENT_DATA:
        ESP_LOGI(TAG_MAIN, "MQTT_EVENT_DATA");
        printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
        getData(event);
        break;
    case MQTT_EVENT_ERROR:
        ESP_LOGI(TAG_MAIN, "MQTT_EVENT_ERROR");
        if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT)
        {
            ESP_LOGI(TAG_MAIN, "Last error code reported from esp-tls: 0x%x", event->error_handle->esp_tls_last_esp_err);
            ESP_LOGI(TAG_MAIN, "Last tls stack error number: 0x%x", event->error_handle->esp_tls_stack_err);
            ESP_LOGI(TAG_MAIN, "Last captured errno : %d (%s)", event->error_handle->esp_transport_sock_errno,
                     strerror(event->error_handle->esp_transport_sock_errno));
        }
        else if (event->error_handle->error_type == MQTT_ERROR_TYPE_CONNECTION_REFUSED)
        {
            ESP_LOGI(TAG_MAIN, "Connection refused error: 0x%x", event->error_handle->connect_return_code);
        }
        else
        {
            ESP_LOGW(TAG_MAIN, "Unknown error type: 0x%x", event->error_handle->error_type);
        }
        break;
    default:
        ESP_LOGI(TAG_MAIN, "Other event id:%d", event->event_id);
        break;
    }
}

void mqtt_init(void)
{
    esp_mqtt_client_config_t mqtt_cfg = {
        .uri = CONFIG_BROKER_URI,
        .cert_pem = (const char *)cert_start,
        .client_cert_pem = (const char *)certificate_start,
        .client_key_pem = (const char *)private_start};

    client = (esp_mqtt_client_handle_t)esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client, (esp_mqtt_event_id_t)ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(client);
}

void task_loop(void *parameters)
{
    ESP_LOGI(TAG_MAIN, "Start TASK Loop.");

    CLed *led;
    led = (CLed *)parameters;

    while (1)
    {
        if (isLed)
        {
            led->toogleLedState(1);
        }
        else
        {
            led->toogleLedState(0);
        }
        led->tick();
        vTaskDelay(CONFIG_BLINK_PERIOD / portTICK_PERIOD_MS);
    }
}

TaskHandle_t xHandle = NULL;

extern "C" void app_main(void)
{

    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    ESP_ERROR_CHECK(example_connect());

    mqtt_init();

    CLed led(LED_PIN);

    ESP_LOGI(TAG_MAIN, "Start Task Create.");
    xTaskCreate(task_loop,    // Task function
                "task_loop",  // Name of task in task scheduler
                1024 * 5,     // Stack size
                (void *)&led, // Parameter send to function
                1,            // Priority
                &xHandle);    // task handler
    ESP_LOGI(TAG_MAIN, "Task Created.");

    while (1)
    {
        // esp_mqtt_client_publish(client, ACTIVATE_TOPIC, "{\"temperature\":12}", 0, 0, 0);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}