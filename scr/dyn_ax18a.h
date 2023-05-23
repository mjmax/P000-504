/*
 * dyn_ax18a.h
 *
 *  Created on: May 19, 2023
 *      Author: Janaka Madhusankha
 */

#ifndef DYN_AX18A_DOT_H
#define DYN_AX18A_DOT_H

#include "types.h"
#include "asciidef.h"

#include <stddef.h>

#define DYN_PACKET_HEADER1                             0xFF
#define DYN_PACKET_HEADER2                             0xFF

#define DYN_PACKET_INST_PING                           0x01
#define DYN_PACKET_INST_READ                           0x02
#define DYN_PACKET_INST_WRITE                          0x03
#define DYN_PACKET_INST_REG_WRITE                      0x04
#define DYN_PACKET_INST_ACTION                         0x05
#define DYN_PACKET_INST_FACTORY_RESET                  0x06
#define DYN_PACKET_INST_REBOOT                         0x08
#define DYN_PACKET_INST_SYNC_WRITE                     0x83
#define DYN_PACKET_INST_SYNC_REG_WRITE                 0x84
#define DYN_PACKET_INST_BULK_READ                      0x92

#define DYN_REG_MODEL_NUMBER                           0x00
#define DYN_REG_FIRMWARE_VERSION                       0x02
#define DYN_REG_ID                                     0x03

#define DYN_REG_BAUD_RATE                              0x04
#define DYN_REG_RETURN_DELAY_TIME                      0x05
#define DYN_REG_CW_ANGLE_LIMIT                         0x06
#define DYN_REG_CCW_ANGLE_LIMIT                        0x08

#define DYN_REG_TEMPERATURE_LIMIT                      0x0B
#define DYN_REG_MIN_VOLTAGE_LIMIT                      0x0C
#define DYN_REG_MAX_VOLTAGE_LIMIT                      0x0D
#define DYN_REG_MAX_TORQUE                             0x0E
#define DYN_REG_STATUS_RETURN_LEVEL                    0x10
#define DYN_REG_ALARM_LED                              0x11
#define DYN_REG_ALARM_SHUTDOWN                         0x12

#define DYN_REG_TORQUE_ENABLE                          0x18
#define DYN_REG_LED                                    0x19
#define DYN_REG_CW_COMPLIANCE_MARGIN                   0x1A
#define DYN_REG_CCW_COMPLIANCE_MARGIN                  0x1B
#define DYN_REG_CW_COMPLIANCE_SLOPE                    0x1C
#define DYN_REG_CCW_COMPLIANCE_SLOPE                   0x1D
#define DYN_REG_GOAL_POSITION                          0x1E

#define DYN_REG_MOVING_SPEED                           0x20

#define DYN_REG_TORQUE_LIMIT                           0x22

#define DYN_REG_PRESENT_POSITION                       0x24

#define DYN_REG_PRESENT_SPEED                          0x26

#define DYN_REG_PRESENT_LOAD                           0x28

#define DYN_REG_PRESENT_VOLTAGE                        0x2A
#define DYN_REG_PRESENT_TEMPERATURE                    0x2B
#define DYN_REG_REGISTERED_INSTRUCTION                 0x2C

#define DYN_REG_MOVING                                 0x2E
#define DYN_REG_LOCK                                   0x2F

#define DYN_REG_PUNCH                                  0x30


typedef struct{
    int8u reg;
    unsigned int size;
    int8u accType;
    int16u defVal;
}dynReg_t;

#define SIZE_1_BYTE          sizeof(int8u)
#define SIZE_2_BYTE          sizeof(int16u)

#define REG_READABLE		(int8u)REG_ACCESSIBILITY_R_STRING
#define REG_WRITABLE		(int8u)REG_ACCESSIBILITY_W_STRING

///////.............combined register mode...................//////
/* combined register modes created by adding individual register modes
 * ex: if 0x02 reg is capable of read and wire the combined reg mode value (RRW) = (REG_READABLE + REG_WRITABLE)
 *
 * (RXX) is the short form of reg combined mode and XX denote Read capability and write capability
 * "_" denote null capability
 */
#define RRW	(REG_READABLE + REG_WRITABLE)
#define RR_	(REG_READABLE)
#define R_W	(REG_WRITABLE)

/*I initial value of the register is not defined*/
#define DEF_NONE                       0xFF

typedef enum {IDLE = 0, HEADER1, HEADER2, PID, PLEN, ERROR, PLOADDATA}dyn_rx_state_t;

struct dyn_packet_t
{
    int8u header1;
    int8u header2;
    int8u pid;
    int8u plen;
    int8u cmd;
    int8u *param;
    int8u checksum;
};


void DynAx18aInit(void);

void dynRxPacketProcess(void);
bool runDynStateMachine(int8u ch);
void dyneReadSerial(int8u ch);
bool dyn_checksum_validate(struct dyn_packet_t *packet);
int8u get_dyn_rx_state(void);
void set_dyn_rx_state(int8u state);
void set_dyn_msg_received(bool status);
bool is_dyn_msg_received(void);
void dyn_packet_init(struct dyn_packet_t *packet, int8u *pdata);
void dyn_rx_packet_load(struct dyn_packet_t *packet);
void dyn_rx_packet_load(struct dyn_packet_t *packet);
void dyn_test_int(void);
void dyn_test_servo(void);
void dyne_test_echo_rx_packet(struct dyn_packet_t *packet);
void dyn_ax_18a_start_tx(void);
void dyn_ax_18a_end_tx(void);
void DynAx18aCheckTxComplete(void);

#endif