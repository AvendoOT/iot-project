#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "led_strip.h"
#include "sdkconfig.h"
#include "CLed.h"

CLed::CLed(int port)
{
    m_pinNumber = (gpio_num_t)port;
    ESP_LOGI(LogName, "Configure LED!");
    pStrip_a = led_strip_init(CONFIG_BLINK_LED_RMT_CHANNEL, port, 1);
    pStrip_a->clear(pStrip_a, 50);
}

void CLed::tick()
{
    if (s_led_state)
    {
        pStrip_a->set_pixel(pStrip_a, 0, 16, 16, 16);
        pStrip_a->refresh(pStrip_a, 100);
    }
    else
    {
        pStrip_a->clear(pStrip_a, 50);
    }
}

void CLed::toogleLedState(int toogle)
{
    s_led_state = toogle;
}