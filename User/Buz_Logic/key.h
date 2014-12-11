#ifndef _KEY_H
#define _KEY_H

#include <stdio.h>
#include "hal_key.h"
#include "hal_rgb_led.h"
#include "delay.h"
#include "gen_protocol.h"
#include "buz_protocol.h"

#define  KEY_DOWN               0x0010
#define  KEY_UP                 0x0020
#define  KEY_LONG               0x0040              // long key press

#define  LONG_KEY_TIMER         200                 // the counter after which is long key press

void HandleKey(void);

#endif /*_KEY_H*/
