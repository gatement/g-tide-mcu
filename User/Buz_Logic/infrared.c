#include "infrared.h"

extern uint8_t  g_val;

void HandleIREvent(uint8_t state)
{
    if(state)
    {
        //printf("infrared event: %i\r\n", 1);
        IncreaseGValue();
    }
    else
    {       
        //printf("infrared event: %i\r\n", 0);
    }
}
