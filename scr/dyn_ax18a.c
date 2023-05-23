/*
 * iocontrol.c
 *
 *  Created on: May 19, 2023
 *      Author: Janaka Madhusankha
 */

#include "avrdet.h"
#include "dyn_ax18a.h"
#include "serial.h"
#include "iocontrol.h"
#include "types.h"
#include "debug.h"

#include <stdio.h>
#include <string.h>
#include <util/delay.h>

#define DYN_AX18A_DIR_PIN       D4

#define SERVO_ADD   0x01
#define PACKET_LEN  0x05

char teststring1[20];
char teststring2[20];
char teststring3[20];

struct dyn_packet_t dyn_rxpacket;
int8u dynRxData[50];
struct dyn_packet_t dyn_txpacket;
int8u dynTxData[50];

int8u dynRxState = IDLE;
bool dynMsgReceived = false;


const int8u dynePacketInst[] = {DYN_PACKET_INST_PING, DYN_PACKET_INST_READ, DYN_PACKET_INST_WRITE, DYN_PACKET_INST_REG_WRITE, DYN_PACKET_INST_ACTION, 
                                DYN_PACKET_INST_FACTORY_RESET, DYN_PACKET_INST_REBOOT, DYN_PACKET_INST_SYNC_WRITE, DYN_PACKET_INST_SYNC_REG_WRITE, DYN_PACKET_INST_BULK_READ};


static const dynReg_t regTable[] = 
{
    {DYN_REG_MODEL_NUMBER               , SIZE_2_BYTE   , RR_   , 18},
    {DYN_REG_FIRMWARE_VERSION           , SIZE_1_BYTE   , RR_   , DEF_NONE},
    {DYN_REG_ID                         , SIZE_1_BYTE   , RRW   , 1},

    {DYN_REG_BAUD_RATE                  , SIZE_1_BYTE   , RRW   , 1},
    {DYN_REG_RETURN_DELAY_TIME          , SIZE_1_BYTE   , RRW   , 250},
    {DYN_REG_CW_ANGLE_LIMIT             , SIZE_2_BYTE   , RRW   , 0},

    {DYN_REG_CCW_ANGLE_LIMIT            , SIZE_2_BYTE   , RRW   , 1023},

    {DYN_REG_TEMPERATURE_LIMIT          , SIZE_1_BYTE   , RRW   , 75},
    {DYN_REG_MIN_VOLTAGE_LIMIT          , SIZE_1_BYTE   , RRW   , 60},
    {DYN_REG_MAX_VOLTAGE_LIMIT          , SIZE_1_BYTE   , RRW   , 140},
    {DYN_REG_MAX_TORQUE                 , SIZE_2_BYTE   , RRW   , 983},

    {DYN_REG_STATUS_RETURN_LEVEL        , SIZE_1_BYTE   , RRW   , 2},
    {DYN_REG_ALARM_LED                  , SIZE_1_BYTE   , RRW   , 36},
    {DYN_REG_ALARM_SHUTDOWN             , SIZE_1_BYTE   , RRW   , 36},

    {DYN_REG_TORQUE_ENABLE              , SIZE_1_BYTE   , RRW   , 0},
    {DYN_REG_LED                        , SIZE_1_BYTE   , RRW   , 0},
    {DYN_REG_CW_COMPLIANCE_MARGIN       , SIZE_1_BYTE   , RRW   , 1},
    {DYN_REG_CCW_COMPLIANCE_MARGIN      , SIZE_1_BYTE   , RRW   , 1},
    {DYN_REG_CW_COMPLIANCE_SLOPE        , SIZE_1_BYTE   , RRW   , 32},
    {DYN_REG_CCW_COMPLIANCE_SLOPE       , SIZE_1_BYTE   , RRW   , 32},
    {DYN_REG_GOAL_POSITION              , SIZE_2_BYTE   , RRW   , DEF_NONE},

    {DYN_REG_MOVING_SPEED               , SIZE_2_BYTE   , RRW   , DEF_NONE},

    {DYN_REG_TORQUE_LIMIT               , SIZE_2_BYTE   , RRW   , 1023},

    {DYN_REG_PRESENT_POSITION           , SIZE_2_BYTE   , RR_   , DEF_NONE},

    {DYN_REG_PRESENT_SPEED              , SIZE_2_BYTE   , RR_   , DEF_NONE},

    {DYN_REG_PRESENT_LOAD               , SIZE_2_BYTE   , RR_   , DEF_NONE},

    {DYN_REG_PRESENT_VOLTAGE            , SIZE_1_BYTE   , RR_   , DEF_NONE},
    {DYN_REG_PRESENT_TEMPERATURE        , SIZE_1_BYTE   , RR_   , DEF_NONE},
    {DYN_REG_REGISTERED_INSTRUCTION     , SIZE_1_BYTE   , RR_   , 0},

    {DYN_REG_MOVING                     , SIZE_1_BYTE   , RR_   , 0},
    {DYN_REG_LOCK                       , SIZE_1_BYTE   , RRW   , 0},
    {DYN_REG_PUNCH                      , SIZE_2_BYTE   , RRW   , 32},
};

void DynAx18aInit(void)
{
    dyn_test_int();
    dyn_packet_init(&dyn_rxpacket, dynRxData);
    dyn_packet_init(&dyn_txpacket, dynTxData);
    set_dyn_msg_received(false);
    pinMode(DYN_AX18A_DIR_PIN, OUTPUT);
}

void dynRxPacketProcess(void)
{
    debug_blink();
    //debug_blink();dyne_test_echo_rx_packet(&dyn_rxpacket);
    set_dyn_msg_received(false);
    // int8u i;
    // int8u id;
    // int8u length;
    // int8u error;
    // int8u checksum;
    // int8u rx_checksum;

    // if(get_dyn_msg_received())
    // {
    //     set_dyn_msg_received(false);
    //     id = dyn_rxpacket.id;
    //     length = dyn_rxpacket.length;
    //     error = dyn_rxpacket.error;
    //     checksum = dyn_rxpacket.checksum;
    //     rx_checksum = dynRxData[length + 4];

    //     if(id == DYN_BROADCAST_ID)
    //     {
    //         // do nothing
    //     }
    //     else
    //     {
    //         if(rx_checksum == checksum)
    //         {
    //             if(error != 0)
    //             {
    //                 // error
    //             }
    //             else
    //             {
    //                 // process data
    //                 for(i = 0; i < length; i++)
    //                 {
    //                     dyn_rxpacket.data[i] = dynRxData[i + 5];
    //                 }
    //             }
    //         }
    //         else
    //         {
    //             // checksum error
    //         }
    //     }
    // }
}

void dyne_test_echo_rx_packet(struct dyn_packet_t *packet)
{
    int8u count = 0;
    /*echo serial received data*/
    SerialPutChar(packet->header1);
    SerialPutChar(packet->header2);
    SerialPutChar(packet->pid);
    SerialPutChar(packet->plen);
    SerialPutChar(packet->cmd);
    for(count = 0; count < (packet->plen - (int8u)2); count++)
    {
        SerialPutChar(dyn_rxpacket.param[count]);
    }
    SerialPutChar(dyn_rxpacket.checksum);
}

bool runDynStateMachine(int8u ch)
{
    static int8u loadCount = 0;
    bool cksmEnable = false;
    bool status = true;
    int8u state = get_dyn_rx_state();

    switch(state)
    {
        case IDLE:
            if(ch == DYN_PACKET_HEADER1)
            {
                dyn_rxpacket.header1 = ch;
                set_dyn_rx_state(HEADER1);
            }
            else
            {
                status = false;
                set_dyn_rx_state(IDLE);
            }
            break;
        case HEADER1:
            if(ch == DYN_PACKET_HEADER2)
            {
                dyn_rxpacket.header2 = ch;
                set_dyn_rx_state(HEADER2);
            }
            else
            {
                status = false;
                set_dyn_rx_state(IDLE);
            }
            break;
        case HEADER2:
            dyn_rxpacket.pid = ch;
            set_dyn_rx_state(PID);
            break;
        case PID:
            dyn_rxpacket.plen = ch;
            set_dyn_rx_state(PLEN);
            break;
        case PLEN:
            dyn_rxpacket.cmd = ch;
            set_dyn_rx_state(ERROR);
            break;
        case ERROR:
            if(dyn_rxpacket.plen > (int8u)2)
            {
                dyn_rxpacket.param[loadCount++] = ch;
                set_dyn_rx_state(PLOADDATA);
            }
            else
                cksmEnable = true;
            break;
        case PLOADDATA:
            if(loadCount >= (dyn_rxpacket.plen - (int8u)2))
            {
                loadCount = 0;
                cksmEnable = true;
            }
            else
                dyn_rxpacket.param[loadCount++] = ch;
            break;
        default:
            set_dyn_rx_state(IDLE);
            break;
    }

    if(cksmEnable)
    {
        //debug_blink();
        dyn_rxpacket.checksum = ch;
        if(dyn_checksum_validate(&dyn_rxpacket))
            set_dyn_msg_received(true);

        set_dyn_rx_state(IDLE);
        cksmEnable = false;
    }

    return status;
}


/*Test Function with simple processing for receive handle*/
void dyneReadSerial(int8u ch)
{
    static bool h1Ok = false;
    static bool h2Ok = false;
    static int8u dCount = 0;

    if(ch == 0xFF)
    {
        h1Ok = true;
        h2Ok = false;
    }

    if(h1Ok && ch == 0xFF)
    {
        h2Ok = true;
    }

    if(h1Ok && h2Ok)
    {
        dynRxData[dCount++] = ch;
    }

    if((dCount >= 1) && (dCount >= dynRxData[1] + 1))
    {
        dCount = 0;
        h1Ok = false;
        h2Ok = false;
        //runDynStateMachine(ch);
        //CommsSendString("test\r\n");
    }
    //SerialPutChar(ch);
}

bool dyn_checksum_validate(struct dyn_packet_t *packet)
{
    int8u checksum = 0,count = 0;

    checksum += packet->pid;
    checksum += packet->plen;
    checksum += packet->cmd;
    for(count = 0; count < (packet->plen - (int8u)2); count++)
        checksum += packet->param[count];
    checksum = ~checksum;

    //SerialPutChar(checksum);
    return (checksum == packet->checksum)? true: false;
}

void set_dyn_msg_received(bool status)
{
    dynMsgReceived = status;
}

bool is_dyn_msg_received(void)
{
    return dynMsgReceived;
}

int8u get_dyn_rx_state(void)
{
    return dynRxState;
}

void set_dyn_rx_state(int8u state)
{
    dynRxState = state;
}

void dyn_packet_init(struct dyn_packet_t *packet, int8u *pdata)
{
    packet->header1 = DYN_PACKET_HEADER1;
    packet->header2 = DYN_PACKET_HEADER2;
    packet->pid = SERVO_ADD;
    packet->plen = 0;
    packet->cmd = 0;
    packet->param = pdata;
    packet->checksum = 0;
}

void dyn_rx_packet_load(struct dyn_packet_t *packet)
{
    dyn_rxpacket.header1 = packet->header1;
    dyn_rxpacket.header2 = packet->header2;
    dyn_rxpacket.pid = packet->pid;
    dyn_rxpacket.plen = packet->plen;
    dyn_rxpacket.cmd = packet->cmd;
    dyn_rxpacket.param = packet->param;
    dyn_rxpacket.checksum = packet->checksum;
}

void dyn_tx_packet_load(struct dyn_packet_t *packet)
{
    dyn_txpacket.header1 = packet->header1;
    dyn_txpacket.header2 = packet->header2;
    dyn_txpacket.pid = packet->pid;
    dyn_txpacket.plen = packet->plen;
    dyn_txpacket.cmd = packet->cmd;
    dyn_txpacket.param = packet->param;
    dyn_txpacket.checksum = packet->checksum;
}

void dyn_test_int(void)
{
    teststring1[0] = (char) DYN_PACKET_HEADER1;
    teststring1[1] = (char) DYN_PACKET_HEADER2;
    teststring1[2] = (char) SERVO_ADD;
    teststring1[3] = (char) PACKET_LEN;
    teststring1[4] = (char) DYN_PACKET_INST_WRITE;
    teststring1[5] = (char) DYN_REG_GOAL_POSITION;
    teststring1[6] = (char) 0xF4;
    teststring1[7] = (char) 0x01;
    teststring1[8] = (char) 0xE3;
    teststring1[9] = '\0';

    teststring2[0] = (char) DYN_PACKET_HEADER1;
    teststring2[1] = (char) DYN_PACKET_HEADER2;
    teststring2[2] = (char) SERVO_ADD;
    teststring2[3] = (char) PACKET_LEN;
    teststring2[4] = (char) DYN_PACKET_INST_WRITE;
    teststring2[5] = (char) DYN_REG_GOAL_POSITION;
    teststring2[6] = (char) 0xFF;
    teststring2[7] = (char) 0x03;
    teststring2[8] = (char) 0xD6;
    teststring2[9] = '\0'; 

    teststring3[0] = (char) DYN_PACKET_HEADER1;
    teststring3[1] = (char) DYN_PACKET_HEADER2;
    teststring3[2] = (char) SERVO_ADD;
    teststring3[3] = (char) 0x04;
    teststring3[4] = (char) DYN_PACKET_INST_READ;
    teststring3[5] = (char) DYN_REG_PRESENT_POSITION;
    teststring3[6] = (char) 0x02;
    teststring3[7] = (char) 0xD2;
    teststring3[8] = '\0';   
}

void dyn_test_servo(void)
{
    static bool flip = true;
    (flip)? CommsSendString(teststring1) : CommsSendString(teststring2);
    flip = !flip;
}

void dyn_test_received_position(void)
{
    CommsSendString(teststring3);
}

void DynAx18aCheckTxComplete(void)
{
    if(get_tx_status())
        digitalWrite(DYN_AX18A_DIR_PIN, LOW);
}

void dyn_ax_18a_end_tx(void)
{
    set_tx_status(TRUE);
    digitalWrite(DYN_AX18A_DIR_PIN, LOW);
}


void dyn_ax_18a_start_tx(void)
{
    set_tx_status(FALSE);
    digitalWrite(DYN_AX18A_DIR_PIN, HIGH);
}
