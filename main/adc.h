#ifndef adc
#define adc

#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"

#include "driver/gpio.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"

#define DEFAULT_VREF    1100        //Use adc2_vref_to_gpio() to obtain a better estimate
#define NO_OF_SAMPLES   64          //Multisampling

static esp_adc_cal_characteristics_t *adc_chars;
// static adc_channel_t channel = ADC_CHANNEL_2;     //GPIO34 if ADC1, GPIO14 if ADC2
static const adc_bits_width_t width = ADC_WIDTH_BIT_12;
static const adc_atten_t atten = ADC_ATTEN_DB_11;
static const adc_unit_t unit = ADC_UNIT_1;

class Adc
{
    public:
        Adc(adc_channel_t channel);
        uint32_t readAdc();
    private:
        void print_char_val_type(esp_adc_cal_value_t val_type);
        void check_efuse(void);
        void init(void);

        adc_channel_t m_channel;
};

#endif