// CLed.h

#ifndef _CLed_h
#define _CLed_h

enum LedStatus{ OFF, ON, BLINK, FAST_BLINK, SLOW_BLINK};


#define LED_BLINK_FAST   250000L
#define LED_BLINK       1000000L
#define LED_BLINK_SLOW  2000000L

class CLed{
    
    public:
        CLed(int port);
        void tick();
        void toogleLedState(int toogle);
    
    private:
        LedStatus m_state = OFF;
        gpio_num_t m_pinNumber;
        led_strip_t *pStrip_a;
        uint8_t s_led_state = 0;
        const char *LogName = "CLed";
};
#endif