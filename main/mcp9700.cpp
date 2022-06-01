#include "mcp9700.h"

MCP9700A::MCP9700A(adc_channel_t channel)
{
    temperature = 0;
    m_adc = new Adc(channel);
}

MCP9700A::~MCP9700A()
{
    
}

void MCP9700A::sendTemp()
{

}

void MCP9700A::readTemp()
{
    uint32_t voltage = m_adc->readAdc();

    temperature = voltage;

    // this->temperature = 38.0;
}

float MCP9700A::getTemp()
{
    return temperature;
}