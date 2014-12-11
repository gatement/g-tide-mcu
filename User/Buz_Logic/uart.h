#ifndef _UART_H
#define _UART_H

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "delay.h"
#include "gen_protocol.h"
#include "buz_protocol.h"

void HandleMsg(void);
void UartInit(void);

#endif /*_UART_H*/
