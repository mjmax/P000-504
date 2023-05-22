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

#define DYN_AX18A_DIR_PIN       D4

#define HEADER      0xFF 
#define SERVO_ADD   0x01
#define CMD_WRITE   0x03
#define GOAL_POS    0x1E
#define PACKET_LEN  0x05

char teststring1[20];
char teststring2[20];

struct dyn_packet_t dyn_rxpacket;
struct dyn_packet_t dyn_txpacket;

int8u dynRxState = IDLE;

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
    pinMode(DYN_AX18A_DIR_PIN, OUTPUT);
}

void runDynStateMachine(int8u *ch)
{
    int8u state = get_dyn_rx_state();
}

int8u get_dyn_rx_state(void)
{
    return dynRxState;
}

void set_dyn_rx_state(int8u state)
{
    dynRxState = state;
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
    teststring1[0] = (char) HEADER;
    teststring1[1] = (char) HEADER;
    teststring1[2] = (char) SERVO_ADD;
    teststring1[3] = (char) PACKET_LEN;
    teststring1[4] = (char) CMD_WRITE;
    teststring1[5] = (char) GOAL_POS;
    teststring1[6] = (char) 0xF4;
    teststring1[7] = (char) 0x01;
    teststring1[8] = (char) 0xE3;
    teststring1[9] = '\0';

    teststring2[0] = (char) HEADER;
    teststring2[1] = (char) HEADER;
    teststring2[2] = (char) SERVO_ADD;
    teststring2[3] = (char) PACKET_LEN;
    teststring2[4] = (char) CMD_WRITE;
    teststring2[5] = (char) GOAL_POS;
    teststring2[6] = (char) 0xFF;
    teststring2[7] = (char) 0x03;
    teststring2[8] = (char) 0xD6;
    teststring2[9] = '\0';    
}

void dyn_test_servo(void)
{
    static bool flip = true;
    (flip)? CommsSendString(teststring1) : CommsSendString(teststring2);
    flip = !flip;
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
