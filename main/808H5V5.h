#ifndef H808H5V5_H_
#define H808H5V5_H_

#include "adc.h"

class H808H5V5
{
    public:
        H808H5V5(adc_channel_t channel);
        virtual ~H808H5V5();
        
        void sendHum();
        void readHum();
        float getHum();

    private:
        float humidity;
        // void readHum();
        Adc *m_adc;
        
};

#endif