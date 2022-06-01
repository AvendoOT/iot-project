#include "adc.h"
 
Adc::Adc(adc_channel_t channel)
{
    m_channel = channel;
}
 
uint32_t Adc::readAdc()
{
    check_efuse();
    init();

    uint32_t adc_reading = 0;
    for (int i = 0; i < NO_OF_SAMPLES; i++) {
        if (unit == ADC_UNIT_1) {
            adc_reading += adc1_get_raw((adc1_channel_t)m_channel);
        } else {
            int raw;
            adc2_get_raw((adc2_channel_t)m_channel, width, &raw);
            adc_reading += raw;
        }
    }
    //adc_reading /= NO_OF_SAMPLES;
    //Convert adc_reading to voltage in mV
    //uint32_t voltage = esp_adc_cal_raw_to_voltage(adc_reading, adc_chars);
    // uint32_t voltage = adc_reading * 950/4095;
    // uint32_t newVoltage = (voltage - 0.958)/0.0307;
    // printf("Raw: %d\tVoltage: %dmV\n", adc_reading, voltage);

    adc_reading /= NO_OF_SAMPLES;
    //Convert adc_reading to voltage in mV
    // uint32_t voltage = esp_adc_cal_raw_to_voltage(adc_reading, adc_chars);
    float voltage = (adc_reading/1023.0)*5.0;
    float percentRH = (voltage-0.958)/0.0307;

    printf("Raw: %d\tVoltage: %fV percent: %f\n", adc_reading, voltage, percentRH);
    
    return voltage;
}

void Adc::init(void)
{
    //Configure ADC
    if (unit == ADC_UNIT_1) {
        adc1_config_width(width);
        adc1_config_channel_atten(adc1_channel_t(m_channel), atten);
    } else {
        adc2_config_channel_atten((adc2_channel_t)m_channel, atten);
    }

    //Characterize ADC
    adc_chars = (esp_adc_cal_characteristics_t*)calloc(1, sizeof(esp_adc_cal_characteristics_t));
    esp_adc_cal_value_t val_type = esp_adc_cal_characterize(unit, atten, width, DEFAULT_VREF, adc_chars);
    print_char_val_type(val_type);
}

void Adc::print_char_val_type(esp_adc_cal_value_t val_type)
{
    if (val_type == ESP_ADC_CAL_VAL_EFUSE_TP) {
        printf("Characterized using Two Point Value\n");
    } else if (val_type == ESP_ADC_CAL_VAL_EFUSE_VREF) {
        printf("Characterized using eFuse Vref\n");
    } else {
        printf("Characterized using Default Vref\n");
    }
}

void Adc::check_efuse(void)
{
    //Check if TP is burned into eFuse
    if (esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_TP) == ESP_OK) {
        printf("eFuse Two Point: Supported\n");
    } else {
        printf("eFuse Two Point: NOT supported\n");
    }
    //Check Vref is burned into eFuse
    if (esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_VREF) == ESP_OK) {
        printf("eFuse Vref: Supported\n");
    } else {
        printf("eFuse Vref: NOT supported\n");
    }
}