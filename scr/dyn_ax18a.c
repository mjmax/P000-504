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
#include <stdlib.h>
#include <util/delay.h>


#define DYN_AX18A_DIR_PIN_0       D5
#define DYN_AX18A_DIR_PIN_1       D2
#define DYN_AX18A_DIR_PIN_2       D3
#define DYN_AX18A_DIR_PIN_3       D4

#define SERVO_ADD   0x01
#define PACKET_LEN  0x05

char teststring1[20];
char teststring2[20];
char teststring3[20];

//int16u motorPos[BUFFER_SIZE];
int16u motorPos[] = {1, 2, 3, 4, 5, 6, 7, 8};
int8u dynErr = 0;
bool readyToTransmit = false;

struct dyn_packet_t dyn_rx0packet;
int8u dynRx0Data[50];
struct dyn_packet_t dyn_tx0packet;
int8u dynTx0Data[50];

#ifdef AVR_ATmega2560
struct dyn_packet_t dyn_rx1packet;
int8u dynRx1Data[50];
struct dyn_packet_t dyn_tx1packet;
int8u dynTx1Data[50];

struct dyn_packet_t dyn_rx2packet;
int8u dynRx2Data[50];
struct dyn_packet_t dyn_tx2packet;
int8u dynTx2Data[50];

struct dyn_packet_t dyn_rx3packet;
int8u dynRx3Data[50];
struct dyn_packet_t dyn_tx3packet;
int8u dynTx3Data[50];
#endif

int8u dynRx0State = IDLE;
int8u dynTx0State = IDLE_TX;
int8u dynRx1State = IDLE;
int8u dynTx1State = IDLE_TX;
int8u dynRx2State = IDLE;
int8u dynTx2State = IDLE_TX;
int8u dynRx3State = IDLE;
int8u dynTx3State = IDLE_TX;
bool dynMsgReceived = false;

bool readyToSample = false; //this flog should move to another c file called sample.c


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
    dyn_packet_init(&dyn_rx0packet, dynRx0Data);
    dyn_packet_init(&dyn_tx0packet, dynTx0Data);
    set_dyn_msg_received(SCI_PORT_0,false);
    set_ready_to_transmit(SCI_PORT_0, true);
    pinMode(DYN_AX18A_DIR_PIN_0, OUTPUT);

#ifdef AVR_ATmega2560
    dyn_packet_init(&dyn_rx1packet, dynRx1Data);
    dyn_packet_init(&dyn_tx1packet, dynTx1Data);
    set_dyn_msg_received(SCI_PORT_1,false);
    set_ready_to_transmit(SCI_PORT_1, true);
    pinMode(DYN_AX18A_DIR_PIN_1, OUTPUT);

    dyn_packet_init(&dyn_rx2packet, dynRx2Data);
    dyn_packet_init(&dyn_tx2packet, dynTx2Data);
    set_dyn_msg_received(SCI_PORT_2,false);
    set_ready_to_transmit(SCI_PORT_2, true);
    pinMode(DYN_AX18A_DIR_PIN_2, OUTPUT);

    dyn_packet_init(&dyn_rx3packet, dynRx3Data);
    dyn_packet_init(&dyn_tx3packet, dynTx3Data);
    set_dyn_msg_received(SCI_PORT_3,false);
    set_ready_to_transmit(SCI_PORT_3, true);
    pinMode(DYN_AX18A_DIR_PIN_3, OUTPUT);
#endif
}

void dynRxPacketProcess(void)
{
    int8u cport = get_current_sci_port();
    struct dyn_packet_t *txPacket = &dyn_tx0packet;
    struct dyn_packet_t *rxPacket = &dyn_rx0packet;
    
    debug_blink();
#ifdef AVR_ATmega2560
    switch(cport)
    {
        case SCI_PORT_0:
            txPacket = &dyn_tx0packet;
            rxPacket = &dyn_rx0packet;
            break;
        case SCI_PORT_1:
            txPacket = &dyn_tx1packet;
            rxPacket = &dyn_rx1packet;
            break;
        case SCI_PORT_2:
            txPacket = &dyn_tx2packet;
            rxPacket = &dyn_rx2packet;
            break;
        case SCI_PORT_3:
            txPacket = &dyn_tx3packet;
            rxPacket = &dyn_rx3packet;
            break;
    }
#endif
    switch(txPacket->param[0])
    {
        case DYN_PACKET_INST_PING:
        case DYN_REG_GOAL_POSITION:
            dynErr = rxPacket->cmd; //dyn_rxpacket.cmd is the corresponding instruction for transmission and corresponding error for reception
            break;
        case DYN_REG_PRESENT_POSITION:
            motorPos[rxPacket->pid] = (rxPacket->param[0] + (rxPacket->param[1] << 8));
            break;
        /*TO DO*/
    }
    set_dyn_msg_received(cport, false);
    set_ready_to_transmit(cport, true);
}

void DynTxPacketProcess(void)
{
    int8u state, cport, beginId;
    static bool highPos[MAX_SCI_PORT] = {true, true, true, true};
    struct dyn_packet_t *packet;

    int8u prevport = get_current_sci_port();
    int8u startPort = SCI_PORT_1;
    int8u endPort = SCI_PORT_3;

    for (cport = startPort; cport <= endPort; cport++)
    {
        set_current_sci_port(cport);
        switch (cport)
        {
            case SCI_PORT_0:
                beginId = 1;
                packet = &dyn_tx0packet;
                break;
            case SCI_PORT_1:
                beginId = 1;
                packet = &dyn_tx1packet;
                break;
            case SCI_PORT_2:
                beginId = 3;
                packet = &dyn_tx2packet;
                break;
            case SCI_PORT_3:
                beginId = 5;
                packet = &dyn_tx3packet;
                break;
            default:
                beginId = 1;
                packet = &dyn_tx0packet;
                break;
        }

        if(is_ready_to_transmit(cport))
        {
            //CommsSendString(SCI_PORT_0, "TestPassed\r\n");
            state = get_dyn_tx_state();
            switch (state)
            {
            case IDLE_TX:
                if(is_ready_to_sample(cport))
                {
                    set_dyn_tx_state(READ_M1_POS);
                    set_ready_to_sample(cport, false);
                }
                break;
            case SET_M1_POS:
                if(highPos[cport])
                {
                    dyn_packet_goal_pos(packet, beginId, 1023);
                }
                else
                    dyn_packet_goal_pos(packet, beginId, 512);
                set_dyn_tx_state(SET_M2_POS);  
                break;
            case SET_M2_POS:
                if(highPos[cport])
                {
                    dyn_packet_goal_pos(packet, (beginId + 1), 1023);
                }
                else
                    dyn_packet_goal_pos(packet, (beginId + 1), 512);
                highPos[cport] = !highPos[cport];
                set_dyn_tx_state(IDLE_TX); 
                break;
            case SET_M3_POS: 
                break;
            case EXECUTE:
                break;
            case READ_M1_POS:
                dyn_packet_read_pos(packet, beginId);
                set_dyn_tx_state(READ_M2_POS);
                break;
            case READ_M2_POS:
                dyn_packet_read_pos(packet, (beginId + 1));
                set_dyn_tx_state(SET_M1_POS); 
                break;
            case READ_M3_POS:  
                break;
            
            default:
                break;
            }

            if(state != IDLE_TX)
            {
                dyn_packet_transmit(cport, packet);
                set_ready_to_transmit(cport, false);
            }
        }
    }
    set_current_sci_port(prevport);
}

bool is_ready_to_sample(int8u port)
{
    return (readyToSample & (1 << port))? true : false;
}

void set_ready_to_sample(int8u port, bool status)
{
    if(status)
        readyToSample |= (1 << port);
    else 
        readyToSample &= ~(1 << port); 
}

void dyne_test_echo_rx_packet(int8u port,struct dyn_packet_t *packet)
{
    int8u count = 0;
    /*echo serial received data*/
    SerialPutChar(port, packet->header1);
    SerialPutChar(port, packet->header2);
    SerialPutChar(port, packet->pid);
    SerialPutChar(port, packet->plen);
    SerialPutChar(port, packet->cmd);
    for(count = 0; count < (packet->plen - (int8u)2); count++)
    {
        SerialPutChar(port, packet->param[count]);
    }
    SerialPutChar(port, packet->checksum);
}

bool runDynStateMachine(int8u ch)
{
    static int8u loadCount[MAX_SCI_PORT] = {0, 0, 0, 0};
    bool cksmEnable = false;
    bool status = true;
    struct dyn_packet_t *packet = &dyn_rx0packet;
    int8u state = get_dyn_rx_state();
    int8u cport = get_current_sci_port();

#ifdef AVR_ATmega2560
    switch (cport)
    {
    case SCI_PORT_0:
        packet = &dyn_rx0packet;
        break;
    case SCI_PORT_1:    
        packet = &dyn_rx1packet;
        break;
    case SCI_PORT_2:    
        packet = &dyn_rx2packet;
        break;
    case SCI_PORT_3:    
        packet = &dyn_rx3packet;
        break;
    }
#endif

    switch(state)
    {
        case IDLE:
            if(ch == DYN_PACKET_HEADER1)
            {
                packet->header1 = ch;
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
                packet->header2 = ch;
                set_dyn_rx_state(HEADER2);
            }
            else
            {
                status = false;
                set_dyn_rx_state(IDLE);
            }
            break;
        case HEADER2:
            packet->pid = ch;
            set_dyn_rx_state(PID);
            break;
        case PID:
            packet->plen = ch;
            set_dyn_rx_state(PLEN);
            break;
        case PLEN:
            packet->cmd = ch;
            set_dyn_rx_state(ERROR);
            break;
        case ERROR:
            if(packet->plen > (int8u)2)
            {
                packet->param[loadCount[cport]++] = ch;
                set_dyn_rx_state(PLOADDATA);
            }
            else
                cksmEnable = true;
            break;
        case PLOADDATA:
            if(loadCount[cport] >= (packet->plen - (int8u)2))
            {
                loadCount[cport] = 0;
                cksmEnable = true;
            }
            else
                packet->param[loadCount[cport]++] = ch;
            break;
        default:
            set_dyn_rx_state(IDLE);
            break;
    }

    if(cksmEnable)
    {
        //debug_blink();
        packet->checksum = ch;
        if(dyn_checksum_validate(packet))
            set_dyn_msg_received(cport,true);

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
        dynRx0Data[dCount++] = ch;
    }

    if((dCount >= 1) && (dCount >= dynRx0Data[1] + 1))
    {
        dCount = 0;
        h1Ok = false;
        h2Ok = false;
        //runDynStateMachine(ch);
        //CommsSendString("test\r\n");
    }
    //SerialPutChar(ch);
}

int8u dyn_cheksum_generate(struct dyn_packet_t *packet)
{
    int8u checksum = 0,count = 0;

    checksum += packet->pid;
    checksum += packet->plen;
    checksum += packet->cmd;
    for(count = 0; count < (packet->plen - (int8u)2); count++)
        checksum += packet->param[count];
    checksum = ~checksum; 
    packet->checksum = checksum;  
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

void set_dyn_tx_state(int8u state)
{
#ifdef AVR_ATmega328P
    dynTx0State = state;
#elif AVR_ATmega2560
    switch (get_current_sci_port())
    {
        case SCI_PORT_0:
            dynTx0State = state;
            break;
        case SCI_PORT_1:
            dynTx1State = state;
            break;
        case SCI_PORT_2:
            dynTx2State = state;
            break;
        case SCI_PORT_3:
            dynTx3State = state;
            break;
        default:
            dynTx0State = state;
            break;
    }
#endif
}

int8u get_dyn_tx_state(void)
{
#ifdef AVR_ATmega328P
    return dynTx0State;
#elif AVR_ATmega2560
    switch (get_current_sci_port())
    {
        case SCI_PORT_0:
            return dynTx0State;
        case SCI_PORT_1:
            return dynTx1State;
        case SCI_PORT_2:
            return dynTx2State;
        case SCI_PORT_3:
            return dynTx3State;
        default:
            return dynTx0State;
    }
#endif
}

void set_ready_to_transmit(int8u port, bool status)
{
    if(status)
        readyToTransmit |= (1 << port);
    else 
         readyToTransmit &= ~(1 << port);  
}

bool is_ready_to_transmit(int8u port)
{
    return (readyToTransmit & (1 << port))? true: false;
}

void set_dyn_msg_received(int8u port,bool status)
{
    if(status)
        dynMsgReceived |= (1 << port);
    else 
        dynMsgReceived &= ~(1 << port);  
}

bool is_dyn_msg_received(int8u port)
{
    return (dynMsgReceived & (1 << port))? true: false;
}

int8u get_dyn_rx_state(void)
{
#ifdef AVR_ATmega328P
    return dynRx0State;
#elif AVR_ATmega2560
    switch (get_current_sci_port())
    {
        case SCI_PORT_0:
            return dynRx0State;
        case SCI_PORT_1:
            return dynRx1State;
        case SCI_PORT_2:
            return dynRx2State;
        case SCI_PORT_3:
            return dynRx3State;
        default:
            return dynRx0State;
    }
#endif
}

void set_dyn_rx_state(int8u state)
{
#ifdef AVR_ATmega328P
    dynRx0State = state;
#elif AVR_ATmega2560
    switch (get_current_sci_port())
    {
        case SCI_PORT_0:
            dynRx0State = state;
            break;
        case SCI_PORT_1:
            dynRx1State = state;
            break;
        case SCI_PORT_2:
            dynRx2State = state;
            break;
        case SCI_PORT_3:
            dynRx3State = state;
            break;
        default:
            dynRx0State = state;
            break;
    }
#endif
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

/*
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
*/

void dyn_test_motor_pos_val(void)
{
    char buffer[100];
    char temp1[6], temp2[6], temp3[6], temp4[6], temp5[6], temp6[6];

    utoa((unsigned int)motorPos[1], temp1, 5);
    utoa((unsigned int)motorPos[2], temp2, 5);
    utoa((unsigned int)motorPos[3], temp3, 5);
    utoa((unsigned int)motorPos[4], temp4, 5);
    utoa((unsigned int)motorPos[5], temp5, 5);
    utoa((unsigned int)motorPos[6], temp6, 5);

    sprintf(buffer, "M1: %s, M2: %s, M3: %s, M4: %s, M5: %s, M6: %s\r\n", temp1, temp2, temp3, temp4, temp5, temp6);
    CommsSendString(SCI_PORT_0,buffer);
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
    (flip)? CommsSendString(SCI_PORT_1, teststring1) : CommsSendString(SCI_PORT_1, teststring2);
    flip = !flip;
}

void dyn_test_received_position(void)
{
    CommsSendString(SCI_PORT_1, teststring3);
}

void dyn_packet_transmit(int8u port, struct dyn_packet_t *packet)
{
    int8u count = 0;
    SerialPutChar(port, packet->header1);
    SerialPutChar(port, packet->header2);
    SerialPutChar(port, packet->pid);
    SerialPutChar(port, packet->plen);
    SerialPutChar(port, packet->cmd);
    for(count = 0; count < (packet->plen - (int8u)2); count++)
        SerialPutChar(port, packet->param[count]);
    SerialPutChar(port, packet->checksum);
}

//void DynAx18aCheckTxComplete(void)
//{
//    if(get_tx_status())
//        digitalWrite(DYN_AX18A_DIR_PIN, LOW);
//}

void dyn_ax_18a_end_tx(int8u port)
{
    set_tx_status(port, TRUE);
#ifdef AVR_ATmega328P
    digitalWrite(DYN_AX18A_DIR_PIN, LOW);
#elif AVR_ATmega2560
    switch (port)
    {
        case SCI_PORT_0:
            digitalWrite(DYN_AX18A_DIR_PIN_0, LOW);
            break;
        case SCI_PORT_1:    
            digitalWrite(DYN_AX18A_DIR_PIN_1, LOW);
            break;
        case SCI_PORT_2:    
            digitalWrite(DYN_AX18A_DIR_PIN_2, LOW);
            break;
        case SCI_PORT_3:    
            digitalWrite(DYN_AX18A_DIR_PIN_3, LOW);
            break;
    }
#endif
}


void dyn_ax_18a_start_tx(int8u port)
{
    set_tx_status(port,FALSE);
    //set_ready_to_transmit(FALSE);
#ifdef AVR_ATmega328P
    digitalWrite(DYN_AX18A_DIR_PIN, HIGH);
#elif AVR_ATmega2560
    switch (port)
    {
        case SCI_PORT_0:
            digitalWrite(DYN_AX18A_DIR_PIN_0, HIGH);
            break;
        case SCI_PORT_1:    
            digitalWrite(DYN_AX18A_DIR_PIN_1, HIGH);
            break;
        case SCI_PORT_2:    
            digitalWrite(DYN_AX18A_DIR_PIN_2, HIGH);
            break;
        case SCI_PORT_3:    
            digitalWrite(DYN_AX18A_DIR_PIN_3, HIGH);
            break;
    }
#endif
}


//Dynemixcel transmit packet creation for different purposes//

void dyn_packet_goal_pos(struct dyn_packet_t *packet, int8u add, int16u value)
{
    int8u cksm = 0;
    int8u dynReg = DYN_REG_GOAL_POSITION;
    int16u setPoint;
    if(value > 1023)
        setPoint = 1023;
    else
        setPoint = value;

    packet->header1 = DYN_PACKET_HEADER1;
    packet->header2 = DYN_PACKET_HEADER2;
    packet->pid = add;
    packet->plen = 0x05;
    packet->cmd = DYN_PACKET_INST_WRITE;
    packet->param[0] = dynReg;
    packet->param[1] = (int8u)(0xFF & setPoint);
    packet->param[2] = (int8u)(0xFF & (setPoint >> 8));
    //cksm = dyn_cheksum_generate(&dyn_txpacket);
    dyn_cheksum_generate(packet);
}

void dyn_packet_read_pos(struct dyn_packet_t *packet, int8u add)
{
    int8u cksm = 0;
    int8u dynReg = DYN_REG_PRESENT_POSITION;

    packet->header1 = DYN_PACKET_HEADER1;
    packet->header2 = DYN_PACKET_HEADER2;
    packet->pid = add;
    packet->plen = 0x04;
    packet->cmd = DYN_PACKET_INST_READ;
    packet->param[0] = dynReg;
    packet->param[1] = 2;//regTable[dynReg].size;
    //cksm = dyn_cheksum_generate(&dyn_txpacket);
    dyn_cheksum_generate(packet);
    //dyn_txpacket.checksum = cksm;
}