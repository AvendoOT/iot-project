// CLed.h

#ifndef _CLed_h
#define _CLed_h

enum LedStatus
{
    OFF,
    ROOM1,
    ROOM2,
    ALL
};

class CLed
{

public:
    CLed(int port, int channel);
    void tick();
    void setLedState(LedStatus x);

private:
    LedStatus m_state = OFF;
    gpio_num_t m_pinNumber;
    led_strip_t *pStrip_a;
    const char *LogName = "CLed";
};
#endif