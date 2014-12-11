#include "gen_protocol.h"

protocol_cmd_error_t                  m_protocol_cmd_error;
protocol_get_mcu_info_ack_t           m_protocol_get_mcu_info_ack;
protocol_heartbeat_ack_t              m_protocol_heartbeat_ack;
protocol_report_module_status_ack_t   m_protocol_report_module_status_ack;
protocol_reboot_mcu_ack_t             m_protocol_reboot_mcu_ack;
protocol_reset_module_t               m_protocol_reset_module;
protocol_set_module_work_mode_t       m_protocol_set_module_work_mode;

extern uint8_t                        uart_buf[256];
extern uint8_t                        uart_wait_ack_time;
extern uint8_t                        uart_got_one_msg;

void GenProtocolInit()
{
    // m_protocol_cmd_error
    memset(&m_protocol_cmd_error, 0, sizeof(protocol_cmd_error_t));
    m_protocol_cmd_error.header.head[0] = 0xFF;
    m_protocol_cmd_error.header.head[1] = 0xFF;
    m_protocol_cmd_error.header.len = ExchangeBytes(sizeof(protocol_cmd_error_t) - 4);
    m_protocol_cmd_error.header.cmd = CMD_MODULE_CMD_ERROR_ACK;

    // m_protocol_get_mcu_info_ack
    memset(&m_protocol_get_mcu_info_ack, 0, sizeof(protocol_get_mcu_info_ack_t));
    m_protocol_get_mcu_info_ack.header.head[0] = 0xFF;
    m_protocol_get_mcu_info_ack.header.head[1] = 0xFF;
    m_protocol_get_mcu_info_ack.header.len = ExchangeBytes(sizeof(protocol_get_mcu_info_ack_t) - 4);
    m_protocol_get_mcu_info_ack.header.cmd = CMD_GET_MCU_INFO_ACK;
    memcpy(m_protocol_get_mcu_info_ack.gen_protocol_ver, GEN_PROTOCOL_VER, 8);
    memcpy(m_protocol_get_mcu_info_ack.buz_protocol_ver, BUZ_PROTOCOL_VER, 8);
    memcpy(m_protocol_get_mcu_info_ack.hard_ver, HARD_VER, 8);
    memcpy(m_protocol_get_mcu_info_ack.soft_ver, SOFT_VER, 8);
    memcpy(m_protocol_get_mcu_info_ack.product_key, PRODUCT_KEY, 32);
    m_protocol_get_mcu_info_ack.bindable_time = ExchangeBytes(BINDABLE_TIMEOUT);

    // m_protocol_heartbeat_ack
    memset(&m_protocol_heartbeat_ack, 0, sizeof(protocol_heartbeat_ack_t));
    m_protocol_heartbeat_ack.header.head[0] = 0xFF;
    m_protocol_heartbeat_ack.header.head[1] = 0xFF;
    m_protocol_heartbeat_ack.header.len = ExchangeBytes(sizeof(protocol_heartbeat_ack_t) - 4);
    m_protocol_heartbeat_ack.header.cmd = CMD_SEND_HEARTBEAT_ACK;

    // m_protocol_report_module_status_ack
    memset(&m_protocol_report_module_status_ack, 0, sizeof(protocol_report_module_status_ack_t));
    m_protocol_report_module_status_ack.header.head[0] = 0xFF;
    m_protocol_report_module_status_ack.header.head[1] = 0xFF;
    m_protocol_report_module_status_ack.header.len = ExchangeBytes(sizeof(protocol_report_module_status_ack_t) - 4);
    m_protocol_report_module_status_ack.header.cmd = CMD_REPORT_MODULE_STATUS_ACK;

    // m_protocol_reboot_mcu_ack
    memset(&m_protocol_reboot_mcu_ack, 0, sizeof(protocol_reboot_mcu_ack_t));
    m_protocol_reboot_mcu_ack.header.head[0] = 0xFF;
    m_protocol_reboot_mcu_ack.header.head[1] = 0xFF;
    m_protocol_reboot_mcu_ack.header.len = ExchangeBytes(sizeof(protocol_reboot_mcu_ack_t) - 4);
    m_protocol_reboot_mcu_ack.header.cmd = CMD_REBOOT_MCU_ACK;

    // m_protocol_reset_module
    memset(&m_protocol_reset_module, 0, sizeof(protocol_reset_module_t));
    m_protocol_reset_module.header.head[0] = 0xFF;
    m_protocol_reset_module.header.head[1] = 0xFF;
    m_protocol_reset_module.header.len = ExchangeBytes(sizeof(protocol_reset_module_t) - 4);
    m_protocol_reset_module.header.cmd = CMD_RESET_MODULE;

    // m_protocol_set_module_work_mode
    memset(&m_protocol_set_module_work_mode, 0, sizeof(protocol_set_module_work_mode_t));
    m_protocol_set_module_work_mode.header.head[0] = 0xFF;
    m_protocol_set_module_work_mode.header.head[1] = 0xFF;
    m_protocol_set_module_work_mode.header.len = ExchangeBytes(sizeof(protocol_set_module_work_mode_t) - 4);
    m_protocol_set_module_work_mode.header.cmd = CMD_SET_MODULE_WORKMODE;
}

/*******************************************************************************
* Function Name  : ExchangeBytes
* Description    : Simulate htons() or ntohs()
* Input          : short int
* Output         : None
* Return         : The short int after exchaged
* Attention      : None
*******************************************************************************/
short ExchangeBytes(short value)
{
    short       tmp_value;
    uint8_t     *index1, *index2;
    
    index1 = (uint8_t *)&tmp_value;
    index2 = (uint8_t *)&value;
    
    *index1 = *(index2 + 1);
    *(index1+1) = *index2;
    
    return tmp_value;
}

/*******************************************************************************
* Function Name  : CheckSum
* Description    : Calculate checksum
* Input          : buf - data address, len - data len
* Output         : None
* Return         : Checksum value
* Attention      : None
*******************************************************************************/
uint8_t CheckSum(uint8_t *buf, int packLen)
{
    int      i;
    uint8_t  sum;

    if(buf == NULL || packLen <= 0) return 0;

    sum = 0;
    for(i = 2; i < packLen-1; i++) sum += buf[i];

    return sum;
}

/*******************************************************************************
* Function Name  : SendErrorAck
* Description    : Send error ack for the request message
* Input          : error_no -  the error code, sn - the message sn
* Output         : None
* Return         : None
* Attention      : None
*******************************************************************************/
void SendErrorAck(uint8_t error_no, uint8_t sn)
{
    m_protocol_cmd_error.header.sn = sn;
    m_protocol_cmd_error.error = error_no;
    m_protocol_cmd_error.sum = CheckSum((uint8_t *)&m_protocol_cmd_error, sizeof(protocol_cmd_error_t));
    
    SendToUart((uint8_t *)&m_protocol_cmd_error, sizeof(protocol_cmd_error_t), 0);
}

/*******************************************************************************
* Function Name  : SendGetMcuInfoAck
* Description    : Send MCU info back to module
* Input          : sn - the message sn
* Output         : None
* Return         : None
* Attention      : None
*******************************************************************************/
void SendGetMcuInfoAck(uint8_t sn)
{
    m_protocol_get_mcu_info_ack.header.sn = sn;
    m_protocol_get_mcu_info_ack.sum = CheckSum((uint8_t *)&m_protocol_get_mcu_info_ack, sizeof(protocol_get_mcu_info_ack_t));
    
    SendToUart((uint8_t *)&m_protocol_get_mcu_info_ack, sizeof(protocol_get_mcu_info_ack_t), 0);
}

/*******************************************************************************
* Function Name  : SendHeartbeatAck
* Description    : Send heartbeat ack back to module
* Input          : sn - the message sn
* Output         : None
* Return         : None
* Attention      : None
*******************************************************************************/
void SendHeartbeatAck(uint8_t sn)
{
    m_protocol_heartbeat_ack.header.sn = sn;
    m_protocol_heartbeat_ack.sum = CheckSum((uint8_t *)&m_protocol_heartbeat_ack, sizeof(protocol_heartbeat_ack_t));
    
    SendToUart((uint8_t *)&m_protocol_heartbeat_ack, sizeof(protocol_heartbeat_ack_t), 0);
}

/*******************************************************************************
* Function Name  : SendHeartbeatAck
* Description    : Send report-module-status ack back to module
* Input          : sn - the message sn
* Output         : None
* Return         : None
* Attention      : None
*******************************************************************************/
void SendReportModuleStatusAck(uint8_t sn)
{
    m_protocol_report_module_status_ack.header.sn = sn;
    m_protocol_report_module_status_ack.sum = CheckSum((uint8_t *)&m_protocol_report_module_status_ack, sizeof(protocol_report_module_status_ack_t));
    
    SendToUart((uint8_t *)&m_protocol_report_module_status_ack, sizeof(protocol_report_module_status_ack_t), 0);
}

/*******************************************************************************
* Function Name  : SendHeartbeatAck
* Description    : Send report-module-status ack back to module
* Input          : sn - the message sn
* Output         : None
* Return         : None
* Attention      : None
*******************************************************************************/
void SendRebootMcuAck(uint8_t sn)
{
    m_protocol_reboot_mcu_ack.header.sn = sn;
    m_protocol_reboot_mcu_ack.sum = CheckSum((uint8_t *)&m_protocol_reboot_mcu_ack, sizeof(protocol_reboot_mcu_ack_t));
    
    SendToUart((uint8_t *)&m_protocol_reboot_mcu_ack, sizeof(protocol_reboot_mcu_ack_t), 0);
}

/*******************************************************************************
* Function Name  : SendResetModule
* Description    : Reset module
* Input          : None
* Output         : None
* Return         : None
* Attention      : None
*******************************************************************************/
void SendResetModule(uint8_t sn)
{
    m_protocol_reset_module.header.sn = sn;
    m_protocol_reset_module.sum = CheckSum((uint8_t *)&m_protocol_reset_module, sizeof(protocol_reset_module_t));
    
    SendToUart((uint8_t *)&m_protocol_reset_module, sizeof(protocol_reset_module_t), 1);
}

void SendSetModuleWorkMode(uint8_t config_method, uint8_t sn)
{
    m_protocol_set_module_work_mode.config_method = config_method;
    m_protocol_set_module_work_mode.header.sn = sn;
    m_protocol_set_module_work_mode.sum = CheckSum((uint8_t *)&m_protocol_set_module_work_mode, sizeof(protocol_set_module_work_mode_t));
    
    SendToUart((uint8_t *)&m_protocol_set_module_work_mode, sizeof(protocol_set_module_work_mode_t), 1);
}

/*******************************************************************************
* Function Name  : SendToUart
* Description    : Send data to uart, waiting for the message ack if the param ack is set
* Input          : buf - data address, packLen - package length, ack - if need to wait for the message ack
* Output         : None
* Return         : None
* Attention      : If param ack is set and doesn't receive message ack, resend package(max 3 times). If data contains 0xFF(except the header), send 0x55 right after 0xFF 
*******************************************************************************/
void SendToUart(uint8_t *buf, uint16_t packLen, uint8_t ack)
{
    uint16_t            i;
    uint8_t             send_num;
    protocol_header_t   sent_header;
    protocol_header_t   rcved_header;
    
    for(i = 0; i < packLen; i++)
    {
        UART1_Send_DATA(buf[i]);
        // If data contains 0xFF(except the header), send 0x55 right after the 0xFF
        if(i >= 2 && buf[i] == 0xFF) UART1_Send_DATA(0x55);
    }
    
    // If ack == 0, no need to wait for ack
    if(ack == 0) return;

    memcpy(&sent_header, buf, sizeof(protocol_header_t));
    memset(&rcved_header, 0, sizeof(protocol_header_t));
    
    uart_wait_ack_time = 0;
    send_num = 1;
    
    while(send_num < MAX_SENDING_TIMES)
    {
        if(uart_wait_ack_time < SENDING_INTERVAL)
        {
            if(uart_got_one_msg)
            {               
                uart_got_one_msg = 0;
                memcpy(&rcved_header, uart_buf, sizeof(protocol_header_t));
                
                // match the sn to adjuge if the sending succeeded
                if(sent_header.cmd == CMD_SEND_MODULE_P0 
                && rcved_header.cmd == CMD_SEND_MODULE_P0_ACK 
                && sent_header.sn == rcved_header.sn) break;

                if(sent_header.cmd == CMD_SET_MODULE_WORKMODE
                && rcved_header.cmd == CMD_SET_MODULE_WORKMODE_ACK
                && sent_header.sn == rcved_header.sn) break;
                
                if(sent_header.cmd == CMD_RESET_MODULE
                && rcved_header.cmd == CMD_RESET_MODULE_ACK
                && sent_header.sn == rcved_header.sn) break;

                if(rcved_header.cmd == CMD_MCU_CMD_ERROR_ACK 
                && sent_header.sn == rcved_header.sn) break;
            }
        }
        else 
        {
            for(i = 0; i < packLen; i++)
            {
                UART1_Send_DATA(buf[i]);
                if(i >= 2 && buf[i] == 0xFF) UART1_Send_DATA(0x55);
            }
            uart_wait_ack_time = 0 ;
            send_num ++;        
        }
    }
}

void HandleRebootMcu()
{
    delay_ms(600);
    NVIC_SystemReset();
}

