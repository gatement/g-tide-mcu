/**
 ******************************************************************************
 * @file    Project/Template/stm32f10x_it.c 
 * @author  MCD Application Team
 * @version V3.1.2
 * @date    09/28/2009
 * @brief   Main Interrupt Service Routines.
 *          This file provides template for all exceptions handler and 
 *          peripherals interrupt service routine.
 ******************************************************************************
 * @copy
 *
 * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
 * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
 * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
 * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
 * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
 * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
 *
 * <h2><center>&copy; COPYRIGHT 2009 STMicroelectronics</center></h2>
 */ 

#include <stm32f10x_it.h>
#include "hal_uart.h"
#include "hal_key.h"
#include "key.h"
#include "infrared.h"

// main vars
extern uint32_t   main_counter;

// key vars
extern uint16_t   key_return;

// uart vars
extern uint8_t    uart_buf[2048];
extern uint16_t   uart_buf_index;
extern uint8_t    uart_msg_len;
extern uint8_t    uart_got_one_msg;
extern uint8_t    uart_wait_ack_time;

// status report
extern uint16_t   check_status_time;
extern uint16_t   report_status_idle_time;

// buz_prototol
extern uint16_t    timer;
extern uint16_t    t_h_timer;

/** @addtogroup Template_Project
 * @{
 */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
 * @brief   This function handles NMI exception.
 * @param  None
 * @retval None
 */
void NMI_Handler(void)
{
}

/**
 * @brief  This function handles Hard Fault exception.
 * @param  None
 * @retval None
 */
void HardFault_Handler(void)
{
    /* Go to infinite loop when Hard Fault exception occurs */
    while (1)
    {
    }
}

/**
 * @brief  This function handles Memory Manage exception.
 * @param  None
 * @retval None
 */
void MemManage_Handler(void)
{
    /* Go to infinite loop when Memory Manage exception occurs */
    while (1)
    {
    }
}

/**
 * @brief  This function handles Bus Fault exception.
 * @param  None
 * @retval None
 */
void BusFault_Handler(void)
{
    /* Go to infinite loop when Bus Fault exception occurs */
    while (1)
    {
    }
}

/**
 * @brief  This function handles Usage Fault exception.
 * @param  None
 * @retval None
 */
void UsageFault_Handler(void)
{
    /* Go to infinite loop when Usage Fault exception occurs */
    while (1)
    {
    }
}

/**
 * @brief  This function handles SVCall exception.
 * @param  None
 * @retval None
 */
void SVC_Handler(void)
{
}

/**
 * @brief  This function handles Debug Monitor exception.
 * @param  None
 * @retval None
 */
void DebugMon_Handler(void)
{
}

/**
 * @brief  This function handles PendSVC exception.
 * @param  None
 * @retval None
 */
void PendSV_Handler(void)
{
}

/**
 * @brief  This function handles SysTick Handler.
 * @param  None
 * @retval None
 */
void SysTick_Handler(void)
{
}

/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/

void USART1_IRQHandler(void)
{  
    uint8_t value;

    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    { 
        USART_ClearITPendingBit(USART1, USART_IT_RXNE);
        value = USART_ReceiveData(USART1);
        if(!uart_got_one_msg)
        {
            // message must be started with 0xffff, or else ignore it
            if(uart_buf_index <= 1 && value != 0xff)
            {
                uart_buf_index = 0;
            }
            else
            {
                uart_buf[uart_buf_index] = value;

                if(uart_buf_index >= 3 && uart_buf[uart_buf_index-1] == 0xff && uart_buf[uart_buf_index] == 0x55)
                {
                    // the sender adds 0x55 after each 0xff except for the header(the very first 2 bytes), so remove it
                }
                else if(uart_buf_index >= 3 && uart_buf[uart_buf_index-1] == 0xff && uart_buf[uart_buf_index] == 0xff)
                {
                    // if received 0xffff, it is the header(the very first two bytes), ignore the received bytes
                    uart_buf_index = 2;
                }
                else
                {
                    // advance the index
                    uart_buf_index ++;
                }

                // calculate msg len
                if(uart_buf_index == 0x04)
                {
                    uart_msg_len = uart_buf[2]*256 + uart_buf[3] + 4; 
                }

                // is one msg received?
                if(uart_buf_index == uart_msg_len)
                {
                    uart_got_one_msg = 1;
                }
            }
        }
    } 
}

/******************************************************************************/
/*                            handler key events                              */
/******************************************************************************/
void TIM3_IRQHandler(void)
{
    static uint8_t  key_state   = 0;                                //按键状态     
    static uint8_t  key_prev    = 0;                                //上一次按键     
    static uint16_t key_delay   = 0;                                //按键连发时间     
    
    uint8_t key_press = NO_KEY;                                     //按键值     

    // trigger every 10 milliseconds
    if(TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)               //检查TIM3更新中断发生与否
    {
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update);                 //清除TIMx更新中断标志 

        uart_wait_ack_time ++;
        check_status_time ++;
        report_status_idle_time ++;
        timer ++;
        t_h_timer ++;

        key_press = Get_Key();  
        switch(key_state)
        {
            case 0:
                if(key_press != NO_KEY)
                {
                    key_state = 1;                                   //转到按键确认              
                    key_prev  = key_press;                           //保存按键状态
                }   
                break;
            case 1:
                if(key_press == key_prev)                            //确认和上次按键相同
                {
                    key_state = 2;                                   //判断按键长按                         
                    key_return = KEY_DOWN | key_prev;
                }   
                else                                                 //按键抬起,是抖动,不响应按键 
                {
                    key_state = 0;
                }
                break;
            case 2:
                if(key_press == NO_KEY)                              //按键释放了 
                {
                    key_state = 0;
                    key_delay = 0; 
                    key_return  = KEY_UP | key_prev;                  //返回按键抬起值
                }
                if (key_press == key_prev)
                {
                    key_delay++; 
                    if(key_delay > LONG_KEY_TIMER) 
                    {               
                        key_delay  = 0;                  
                        key_return = KEY_LONG | key_prev;              //返回长按后的值                
                    }
                }
                break;
            default:
                break;
        } 
    }
} 

void EXTI15_10_IRQHandler(void) 
{
    EXTI->EMR &= (uint32_t)~(1<<1);                                     //屏蔽中断事件

    while(EXTI_GetITStatus(EXTI_Line12) != RESET) 
    {       
        if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_12))
        {
            HandleIREvent(0);
        }
        else
        {       
            HandleIREvent(1);
        }

        EXTI_ClearITPendingBit(EXTI_Line12);
    }

    EXTI->EMR |= (uint32_t)(1<<1);                                      //开启中断事件  
}

/**
 * @}
 */ 
/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/
