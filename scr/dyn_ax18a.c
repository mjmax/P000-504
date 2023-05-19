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

void DynAx18aInit(void)
{
    dyn_test_int();
    pinMode(DYN_AX18A_DIR_PIN, OUTPUT);
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
