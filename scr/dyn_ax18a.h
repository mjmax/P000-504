/*
 * dyn_ax18a.h
 *
 *  Created on: May 19, 2023
 *      Author: Janaka Madhusankha
 */

#ifndef DYN_AX18A_DOT_H
#define DYN_AX18A_DOT_H

#include "types.h"


void DynAx18aInit(void);
void dyn_test_int(void);
void dyn_test_servo(void);
void dyn_ax_18a_start_tx(void);
void dyn_ax_18a_end_tx(void);
void DynAx18aCheckTxComplete(void);

#endif