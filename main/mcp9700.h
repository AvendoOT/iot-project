#ifndef MCP9700A_H_
#define MCP9700A_H_

#include "adc.h"
#include <time.h>

class MCP9700A
{
    public:
        MCP9700A();
        virtual ~MCP9700A();
        
        void sendTemp();
        void readTemp();
        float getTemp();

    private:
        double m_temperature;
        // Adc *m_adc;
};

#endif