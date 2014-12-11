#include "hal_key.h"

/*******************************************************************************
 * Function Name  : KEY_GPIO_Init
 * Description    : Configure GPIO Pin
 * Input          : None
 * Output         : None
 * Return         : None
 * Attention      : None
 *******************************************************************************/
void KEY_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(GPIO_KEY1_CLK | GPIO_KEY2_CLK | GPIO_KEY3_CLK | GPIO_KEY4_CLK, ENABLE);

    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 

    GPIO_InitStructure.GPIO_Pin = GPIO_KEY1_PIN;
    GPIO_Init(GPIO_KEY1_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_KEY2_PIN;
    GPIO_Init(GPIO_KEY2_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_KEY3_PIN;
    GPIO_Init(GPIO_KEY3_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_KEY4_PIN;
    GPIO_Init(GPIO_KEY4_PORT, &GPIO_InitStructure);
}

/*******************************************************************************
 * Function Name  : TIM3_Int_Init
 * Description    : TIM3 initialization function
 * Input          : None
 * Output         : None
 * Return         : None
 * Attention        : None
 *******************************************************************************/
void TIM3_Int_Init(u16 arr,u16 psc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);                        //时钟使能

    //定时器TIM3初始化
    TIM_TimeBaseStructure.TIM_Period = arr;                                     //设置在下一个更新事件装入活动的自动重装载寄存器周期的值    
    TIM_TimeBaseStructure.TIM_Prescaler =psc;                                   //设置用来作为TIMx时钟频率除数的预分频值
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;                     //设置时钟分割:TDTS = Tck_tim
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;                 //TIM向上计数模式
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);                             //根据指定的参数初始化TIMx的时间基数单位

    TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE );                                   //使能指定的TIM3中断,允许更新中断

    //中断优先级NVIC设置
    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;                             //TIM3中断
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;                   //先占优先级0级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;                          //从优先级3级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                             //IRQ通道被使能
    NVIC_Init(&NVIC_InitStructure);                                             //初始化NVIC寄存器 
    TIM_Cmd(TIM3, ENABLE);                                                      //使能TIMx                   
}

/*******************************************************************************
 * Function Name  : Get_Key
 * Description    : Read the KEY state
 * Input          : None
 * Output         : None
 * Return         : uint8_t KEY state
 * Attention      : None
 *******************************************************************************/
uint8_t Get_Key(void) 
{
    if(!GPIO_ReadInputDataBit(GPIO_KEY1_PORT, GPIO_KEY1_PIN))
    {
        return PRESS_KEY1;
    }   
    else if(!GPIO_ReadInputDataBit(GPIO_KEY2_PORT, GPIO_KEY2_PIN))
    {
        return PRESS_KEY2;
    }
    else if(!GPIO_ReadInputDataBit(GPIO_KEY3_PORT, GPIO_KEY3_PIN))
    {
        return PRESS_KEY3;
    }
    else if(!GPIO_ReadInputDataBit(GPIO_KEY4_PORT, GPIO_KEY4_PIN))
    {
        return PRESS_KEY4;
    }
    else
        return NO_KEY;
}
