#include "mcp9700.h"

MCP9700A::MCP9700A()
{
    m_temperature = 0;
    // m_adc = new Adc(channel);
}

MCP9700A::~MCP9700A()
{
    
}

void MCP9700A::sendTemp()
{

}

void MCP9700A::readTemp()
{
    FILE *fp;
    char str[5];

    fp = fopen ("temperature.txt","r");

    srand(time(NULL));
    int position = rand() % 21;
    fseek(fp, position, SEEK_SET);
    fgets(str, 5, fp);

    m_temperature = strtod(str, NULL);

    fclose(fp);
}

float MCP9700A::getTemp()
{
    return m_temperature;
}