#include "808H5V5.h"

H808H5V5::H808H5V5(adc_channel_t channel)
{
    humidity = 0;
    m_adc = new Adc(channel);
}

H808H5V5::~H808H5V5()
{
}

void H808H5V5::sendHum()
{
}

float H808H5V5::readHum()
{
    return m_adc->readAdc();
}

float H808H5V5::getHum()
{
    return humidity;
}