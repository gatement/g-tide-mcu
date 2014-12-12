#include "buz_protocol.h"

protocol_write_t                   m_protocol_write;
protocol_write_ack_t               m_protocol_write_ack;
protocol_read_t                    m_protocol_read;
protocol_mcu_status_t              m_protocol_mcu_status;

uint16_t                           report_status_idle_time;
uint16_t                           decrease_value_time;
uint8_t                            gvalue_honey; // I push up honey's value and send to her/him
uint8_t                            gvalue_honey_last;
uint8_t                            gvalue_my;    // Honey push up my value and send it to me
uint8_t                            high;         // If reach high or not

extern uint8_t                     uart_msg_sn;

void BuzProtocolInit()
{
    gvalue_honey = 0;
    gvalue_honey_last = 255; // make it differ with gvalue_honey
    gvalue_my = 0;
    high = 0;

    report_status_idle_time = 0;
    decrease_value_time = 0;

    // m_protocol_write_ack
    memset(&m_protocol_write_ack, 0, sizeof(protocol_write_ack_t));
    m_protocol_write_ack.header.head[0] = 0xFF;
    m_protocol_write_ack.header.head[1] = 0xFF;
    m_protocol_write_ack.header.len = ExchangeBytes(sizeof(protocol_write_ack_t) - 4);
    m_protocol_write_ack.header.cmd = CMD_SEND_MCU_P0_ACK;

    // m_protocol_mcu_status
    memset(&m_protocol_mcu_status, 0, sizeof(protocol_mcu_status_t));
    m_protocol_mcu_status.header.head[0] = 0xFF;
    m_protocol_mcu_status.header.head[1] = 0xFF;
    m_protocol_mcu_status.header.len = ExchangeBytes(sizeof(protocol_mcu_status_t) - 4);
}

void ReportStatus(uint8_t tag, uint8_t sn)
{
    if(tag == QUERY_STATUS)
    {
        m_protocol_mcu_status.header.sn = sn;
        m_protocol_mcu_status.header.cmd = CMD_SEND_MCU_P0_ACK;
        m_protocol_mcu_status.sub_cmd = SUB_CMD_READ_ACK;
        m_protocol_mcu_status.gvalue = gvalue_my;
        m_protocol_mcu_status.sum = CheckSum((uint8_t *)&m_protocol_mcu_status, sizeof(protocol_mcu_status_t));
        SendToUart((uint8_t *)&m_protocol_mcu_status, sizeof(protocol_mcu_status_t), 0);
    }
    else if(tag == REPORT_STATUS)
    {
        m_protocol_mcu_status.header.sn = uart_msg_sn++;
        m_protocol_mcu_status.header.cmd = CMD_SEND_MODULE_P0;
        m_protocol_mcu_status.sub_cmd = SUB_CMD_REPORT_STATUS;
        m_protocol_mcu_status.gvalue = gvalue_honey;
        m_protocol_mcu_status.sum = CheckSum((uint8_t *)&m_protocol_mcu_status, sizeof(protocol_mcu_status_t));
        SendToUart((uint8_t *)&m_protocol_mcu_status, sizeof(protocol_mcu_status_t), 1);

        // save the last value
        gvalue_honey_last = gvalue_honey;
    }

}

void HandleModuleStatus(uint16_t status)
{
    //uint8_t rssi = (status >> 6) & 0x07;
    //printf("wifi status: 0x%x, RSSI: 0x%x\r\n", status, rssi);
    //LED_RGB_Control(0, rssi*32, 0); // use the green light to indicate wifi RSSI
}

void HandleControl(uint8_t *buf)
{
    memcpy(&m_protocol_write, buf, sizeof(protocol_write_t));
    
    // control and save current value
    if(m_protocol_write.flags & 0x01)
    {
        gvalue_my = m_protocol_write.gvalue;
        if(gvalue_my > G_VAL_MAX) gvalue_my = G_VAL_MAX;
    }

    // send ack
    m_protocol_write_ack.header.sn = m_protocol_write.header.sn;
    m_protocol_write_ack.sum = CheckSum((uint8_t *)&m_protocol_write_ack, sizeof(protocol_write_ack_t));
    SendToUart((uint8_t *)&m_protocol_write_ack, sizeof(protocol_write_ack_t), 0);
}

void HandleQueryStatus(uint8_t *buf)
{
    memcpy(&m_protocol_read, buf, sizeof(protocol_read_t));
    ReportStatus(QUERY_STATUS, m_protocol_read.header.sn);
}

void HandleBuzCmd(protocol_header_t protocol_header, uint8_t *buf)
{
    uint8_t sub_cmd = buf[8];
    switch(sub_cmd)
    {
        case SUB_CMD_WRITE:
            //printf("SUB_CMD_WRITE\r\n");
            HandleControl(buf);
            break;
        case SUB_CMD_READ:
            //printf("SUB_CMD_READ\r\n");
            HandleQueryStatus(buf);
            break;
        default:
            //printf("SUB_CMD_UNIDENTIFIED\r\n");
            SendErrorAck(ERROR_OTHER, protocol_header.sn);
            break;
    }
}

void High(uint16_t run_time, uint16_t stop_time)
{
    Motor_Control(100, 0);
    LED_RGB_Control(255, 0, 0);
    delay_ms(run_time);
    Motor_Control(0, 0);
    LED_RGB_Control(0, 0, 0);
    delay_ms(stop_time);
}

void UploadStatus()
{
    // send honey's value every 2 seconds 
    if(high == 0 && report_status_idle_time > 150 && gvalue_honey != gvalue_honey_last)
    {
        ReportStatus(REPORT_STATUS, 0);
        report_status_idle_time = 0;
    }
}

void Display()
{
    if(high == 0)
    {
        // high if my value and honey's value are reach the max value
        if(gvalue_my >= G_VAL_MAX)
        {
            ReportStatus(REPORT_STATUS, 0);
            high = 1;

            High(500, 4000);
            High(500, 4000);
            High(500, 4000);
            High(500, 4000);
            High(500, 4000);
            High(500, 4000);
            while(gvalue_my > 0)
            {
                gvalue_my -= 1;
                LED_RGB_Control(gvalue_my, 0, 0);
                delay_ms(8);
            }
        }
        else
        {
            Motor_Control(gvalue_my/3, 0);
            LED_RGB_Control(0, gvalue_my, 0);
        }
    }
}

void IncreaseGValue(uint8_t step)
{
    if(gvalue_honey + step < G_VAL_MAX) 
        gvalue_honey += step;
    else
        gvalue_honey = G_VAL_MAX - 1;
}

void DecreaseGValue()
{
    if(decrease_value_time > 150)
    {
        decrease_value_time = 0;

        if(gvalue_honey > 248)
            gvalue_honey -= 2;
        else if(gvalue_honey > 240)
            gvalue_honey -= 5;
        else if(gvalue_honey > 200)
            gvalue_honey -= 8;
        else if(gvalue_honey > 150)
            gvalue_honey -= 7;
        else if(gvalue_honey > 100)
            gvalue_honey -= 5;
        else if(gvalue_honey > 50)
            gvalue_honey -= 4;
        else if(gvalue_honey > 10)
            gvalue_honey -= 3;
        else if(gvalue_honey > 5)
            gvalue_honey -= 2;
        else if(gvalue_honey > 1)
            gvalue_honey -= 1;
        else
            gvalue_honey = 0;
    }
}

void BuzTick()
{
    UploadStatus();
    Display();
    DecreaseGValue();
}
