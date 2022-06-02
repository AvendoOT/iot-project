#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "led_strip.h"
#include "sdkconfig.h"
#include "CLed.h"

CLed::CLed(int port, int channel)
{
    m_pinNumber = (gpio_num_t)port;
    ESP_LOGI(LogName, "Configure LED!");
    pStrip_a = led_strip_init(channel, port, 1);
    pStrip_a->clear(pStrip_a, 50);
}

void CLed::tick()
{
    switch (m_state)
    {
    case OFF:
        pStrip_a->clear(pStrip_a, 50);
        break;
    case ROOM1:
        pStrip_a->set_pixel(pStrip_a, 0, 255, 0, 0);
        pStrip_a->refresh(pStrip_a, 100);
        break;
    case ROOM2:
        pStrip_a->set_pixel(pStrip_a, 0, 0, 255, 0);
        pStrip_a->refresh(pStrip_a, 100);
        break;
    case ALL:
        pStrip_a->set_pixel(pStrip_a, 0, 0, 0, 255);
        pStrip_a->refresh(pStrip_a, 100);
        break;
    default:
        pStrip_a->clear(pStrip_a, 50);
        break;
    }
}

void CLed::setLedState(LedStatus x)
{
    m_state = x;
}