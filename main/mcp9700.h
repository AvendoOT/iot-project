#ifndef MCP9700A_H_
#define MCP9700A_H_

#include "adc.h"

class MCP9700A
{
    public:
        MCP9700A(adc_channel_t channel);
        virtual ~MCP9700A();
        
        void sendTemp();
        void readTemp();
        float getTemp();

    private:
        float temperature;
        // void readTemp();
        Adc *m_adc;
        
};

#endif