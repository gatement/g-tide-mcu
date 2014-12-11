#include "infrared.h"

extern uint8_t  g_val;

void HandleIREvent(uint8_t state)
{
    if(state)
    {
        //printf("infrared event: %i\r\n", 1);
        g_val ++;
        if(g_val > G_VAL_MAX) g_val = 0;
    }
    else
    {       
        //printf("infrared event: %i\r\n", 0);
    }
}
