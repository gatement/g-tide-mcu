#include "key.h"

uint16_t key_return = NO_KEY;

extern uint8_t  uart_msg_sn;
extern uint8_t  g_val;

/*******************************************************************************
 * Function Name  : HandleKey
 * Description    : The key handler function
 * Input          : None
 * Output         : None
 * Return         : None
 * Attention      : None
 *******************************************************************************/
void HandleKey(void)
{
    // long key1 pressure to reset module
    if(key_return & PRESS_KEY1)
    {
        if (key_return & KEY_LONG)
        {
            LED_RGB_Control(0,0,0);
            SendResetModule(uart_msg_sn++);
        }
        else if (key_return & KEY_DOWN)
        {
            LED_RGB_Control(0,0,50);
        }
        else // KEY_DOWN
        {
            LED_RGB_Control(0,0,0);
        }

        key_return = 0;
    }

    // press key2 to config module
    if(key_return & PRESS_KEY2)
    {
        if (key_return & KEY_DOWN)
        {
            LED_RGB_Control(0,0,50);
        }
        else if (key_return & KEY_UP)
        {
            LED_RGB_Control(0,0,0);
            SendSetModuleWorkMode(CONFIG_METHOD_AIRLINK, uart_msg_sn++);
        }

        key_return = 0;
    }

    // press key3 to config module
    if(key_return & PRESS_KEY3)
    {
        if (key_return & KEY_UP)
        {
            g_val ++;
            if(g_val > G_VAL_MAX) g_val = 0;
        }

        key_return = 0;
    }
}
