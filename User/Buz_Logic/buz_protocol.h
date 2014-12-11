#ifndef _BUZ_PROTOCOL_H
#define _BUZ_PROTOCOL_H

#include "delay.h"
#include "hal_key.h"
#include "hal_rgb_led.h"
#include "hal_motor.h"
#include "hal_temp_hum.h"
#include "hal_infrared.h"
#include "gen_protocol.h"

// sub cmd
#define     SUB_CMD_WRITE                         0x01
#define     SUB_CMD_READ                          0x02
#define     SUB_CMD_READ_ACK                      0x03
#define     SUB_CMD_REPORT_STATUS                 0x04

#define     REPORT_STATUS                         1
#define     QUERY_STATUS                          2

#define     G_VAL_MAX                             50 

typedef struct _protocol_write_t                protocol_write_t;
typedef struct _protocol_write_ack_t            protocol_write_ack_t;
typedef struct _protocol_read_t                 protocol_read_t;
typedef struct _protocol_mcu_status_t           protocol_mcu_status_t;

__packed struct _protocol_write_t
{
    protocol_header_t               header;
    uint8_t                         sub_cmd;
    uint8_t                         flags;
    uint8_t                         power;
    uint8_t                         motor;
    uint8_t                         sum;    
};

__packed struct _protocol_write_ack_t
{
    protocol_header_t               header;
    uint8_t                         sum;    
};

__packed struct _protocol_read_t
{
    protocol_header_t               header;
    uint8_t                         sub_cmd;
    uint8_t                         sum; 
};

__packed struct _protocol_mcu_status_t
{
    protocol_header_t               header;
    uint8_t                         sub_cmd;
    uint8_t                         power;
    uint8_t                         motor;
    uint8_t                         sum;    
};

void HandleModuleStatus(uint16_t status);
void HandleBuzCmd(protocol_header_t protocol_header, uint8_t *buf);
void BuzProtocolInit(void);
void CheckStatus(void);
void UpdateStatus(void);

#endif /*_BUZ_PROTOCOL_H*/
