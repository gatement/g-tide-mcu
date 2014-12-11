#ifndef _HAL_KEY_H
#define _HAL_KEY_H

#include <stm32f10x.h>

#define GPIO_KEY1_CLK       RCC_APB2Periph_GPIOB
#define GPIO_KEY1_PORT      GPIOB
#define GPIO_KEY1_PIN       GPIO_Pin_12

#define GPIO_KEY2_CLK       RCC_APB2Periph_GPIOB
#define GPIO_KEY2_PORT      GPIOB
#define GPIO_KEY2_PIN       GPIO_Pin_13

#define GPIO_KEY3_CLK       RCC_APB2Periph_GPIOB
#define GPIO_KEY3_PORT      GPIOB
#define GPIO_KEY3_PIN       GPIO_Pin_14

#define GPIO_KEY4_CLK       RCC_APB2Periph_GPIOB
#define GPIO_KEY4_PORT      GPIOB
#define GPIO_KEY4_PIN       GPIO_Pin_15

#define NO_KEY                  0x0000
#define PRESS_KEY1              0x0001
#define PRESS_KEY2              0x0002
#define PRESS_KEY3              0x0004
#define PRESS_KEY4              0x0008

void      KEY_GPIO_Init(void);
void      TIM3_Int_Init(u16 arr,u16 psc);
uint8_t   Get_Key(void);

#endif /*_HAL_KEY_H*/
