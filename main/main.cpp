#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
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
#include "driver/temp_sensor.h"
#include "808H5V5.h"
#include "CLed.h"

#define ADC_CHANNEL_HUMIDITY ADC_CHANNEL_1
#define LED_PIN_NUM 8
#define ROOM_1_ACTUATOR_TOPIC "actuator/22"
#define ROOM_2_ACTUATOR_TOPIC "actuator/23"
#define SEND_DATA_TOPIC "device/22/data"
#define ROOM_1_ACTUATOR_NUM 22
#define ROOM_2_ACTUATOR_NUM 23

static const char *TAG_MAIN = "iot-project";

bool isOpen_room1 = false;
bool isOpen_room2 = false;

extern const uint8_t cert_start[] asm("_binary_AmazonRootCA1_pem_start");
extern const uint8_t cert_end[] asm("_binary_AmazonRootCA1_pem_end");
extern const uint8_t certificate_start[] asm("_binary_certificate_pem_crt_start");
extern const uint8_t certificate_end[] asm("_binary_certificate_pem_crt_end");
extern const uint8_t private_start[] asm("_binary_private_pem_key_start");
extern const uint8_t private_end[] asm("_binary_private_pem_key_end");

char *json_data_template = (char *)malloc(100 * sizeof(char));

esp_mqtt_client_handle_t client;
float temperature_reading = 0.0;
float humidity_reading = 0.0;

void sendData(void *parameters)
{
    while (1)
    {
        vTaskDelay((CONFIG_BLINK_PERIOD * 4) / portTICK_PERIOD_MS);
        sprintf(json_data_template, "{\"temperature\":%.2f, \"humidity\":%f}", temperature_reading, humidity_reading);
        esp_mqtt_client_publish(client, SEND_DATA_TOPIC, json_data_template, 0, 0, 0);
    }
}

void temperature_sensor_task(void *arg)
{
    char reading[64];
    temp_sensor_config_t temp_sensor = TSENS_CONFIG_DEFAULT();
    temp_sensor_get_config(&temp_sensor);
    ESP_LOGI(TAG_MAIN, "default dac %d, clk_div %d", temp_sensor.dac_offset, temp_sensor.clk_div);
    temp_sensor.dac_offset = TSENS_DAC_DEFAULT;
    temp_sensor_set_config(temp_sensor);
    temp_sensor_start();
    ESP_LOGI(TAG_MAIN, "Temperature sensor started");
    while (1)
    {
        vTaskDelay(CONFIG_BLINK_PERIOD / portTICK_PERIOD_MS);
        temp_sensor_read_celsius(&temperature_reading);
        snprintf(reading, sizeof reading, "%f", temperature_reading);
        ESP_LOGI(TAG_MAIN, "Temperature out celsius %f°C", temperature_reading);
    }
    vTaskDelete(NULL);
}

void humidity_sensor_task(void *parameters)
{
    H808H5V5 *humidity_sensor;
    humidity_sensor = (H808H5V5 *)parameters;

    while (1)
    {
        vTaskDelay(CONFIG_BLINK_PERIOD / portTICK_PERIOD_MS);
        humidity_reading = humidity_sensor->readHum();
    }
    vTaskDelete(NULL);
}

void getData(esp_mqtt_event_handle_t event)
{
    char topicName[2] = {event->topic[event->topic_len - 2],
                         event->topic[event->topic_len - 1]};
    int topicNum = atoi(topicName);
    if (topicNum == ROOM_1_ACTUATOR_NUM)
    {
        isOpen_room1 = !isOpen_room1;
    }
    else if (topicNum == ROOM_2_ACTUATOR_NUM)
    {
        isOpen_room2 = !isOpen_room2;
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
        msg_id = esp_mqtt_client_subscribe(client, ROOM_1_ACTUATOR_TOPIC, 0);
        ESP_LOGI(TAG_MAIN, "sent subscribe successful, msg_id=%d", msg_id);
        msg_id = esp_mqtt_client_subscribe(client, ROOM_2_ACTUATOR_TOPIC, 0);
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

extern "C" void mqtt_init(void)
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
        if (!isOpen_room1 && !isOpen_room2)
        {
            led->setLedState(LedStatus::OFF);
        }
        else if (isOpen_room1 && !isOpen_room2)
        {
            led->setLedState(LedStatus::ROOM1);
        }
        else if (isOpen_room2 && !isOpen_room1)
        {
            led->setLedState(LedStatus::ROOM2);
        }
        else
        {
            led->setLedState(LedStatus::ALL);
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

    H808H5V5 humidity_sensor(ADC_CHANNEL_HUMIDITY);

    CLed room(LED_PIN_NUM, 0);

    ESP_LOGI(TAG_MAIN, "Start Task Create.");

    xTaskCreate(temperature_sensor_task, "temperature_sensor_task", 2048, NULL, 5, NULL);

    xTaskCreate(humidity_sensor_task, "temperature_sensor_task", 2048, (void *)&humidity_sensor, 5, NULL);

    xTaskCreate(task_loop,
                "task_loop",
                1024 * 5,
                (void *)&room,
                1,
                &xHandle);

    xTaskCreate(sendData, "send_data_task", 2048, NULL, 5, NULL);

    ESP_LOGI(TAG_MAIN, "Task Created.");

    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}