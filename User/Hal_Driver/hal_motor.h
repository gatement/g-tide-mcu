#ifndef _HAL_MOTOR_H
#define _HAL_MOTOR_H

#include <stdio.h>
#include <stm32f10x.h>
#include "sys.h"
#include "sa_gpio.h"

#define MOT1 PBout(8)
#define MOT2 PBout(9)

#define MOTOR_ARR 899 //8kHZ
#define MOTOR_MAX 100

void Motor_Init(void);

// clockwiseSpeed: 0 ~ 100
// counterClockwiseSpeed: 0 ~ 100
void Motor_Control(u8 clockwiseSpeed, u8 counterClockwiseSpeed);

#endif /*_HAL_MOTOR_H*/
