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

void H808H5V5::readHum()
{
    // uint32_t voltage = m_adc->readAdc();

    this->humidity = 38.0;
}

float H808H5V5::getHum()
{
    return humidity;
}