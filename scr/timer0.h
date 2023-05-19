/***********************************************************************
MODULE:    Timer0
VERSION:   1.01
NOTES:
  The prescaler is shared between Timer0 & Timer1, so care should be
  taken when resetting the prescaler unit.

  For the M400 modules, the timers are utilised as follows:
   Timer0 (8 bit)  DIN   (uses prescaler divider)
   Timer1 (16 bit) AOUT  (No prescaler divider)
   Timer2 (8 bit)  DOUT  (uses prescaler divider- will reset to re-sync)

***********************************************************************/

#ifndef _TIMER0H_
#define _TIMER0H_


////////////////////////////////////////////////////////////////////////
// NOTES: Timer0 reload
//        The operation is an 8 bit up counter, with an overflow
//        interrupt being generated at transition all 1's to all 0's.
//        Thus the RELOAD value must be 2^8-COUNT_TICKS
//        With the CLK being used (7372800Hz or 8000000Hz), the prescaler
//        should be set to give count range of 1-255
//
//        So the prescaler should be 1:1   (0x01),
//                                   /8    (0x02),
//                                   /64   (0x03),
//                                   /256  (0x04),
//                                   /1024 (0x05)
//
//        For 1000Hz (1ms) resolution, we have the following:
//        Total = CLK/TICK_FREQ = 7372800/1000 = 7372.8 which must then be
//        prescaled to fit in 8 bit counter: /64 gives 115.2 (115)
//        RELOAD_COUNT = 2^8-COUNTS = 256-115 = 141
//        actual frequency is 1001.7Hz
//
//        For a 12.9024MHz crystal, the actual delay will be 1.00198ms (or 0.2% error)
//        so for a delay of 255ms, the error would be
//
//        For a 18.432MHz crystal, the following needs to be changed if this timer is
//        used (it isn't currently).  1ms isn't achievable with this divider.
//        Either /256 or /1024 will work however.
//
//        I've reworked the timer initialisation so it calculates the prescaler
//        based on the requested tick frequency & input crystal frequency.
//        The DIN/AOUT/DOUT code needs to be checked, as they use the same
//        prescaler.
#define T0_TICK_FREQUENCY 1000UL


////////////////////////////////////////////////////////////////////////
// NOTES: Timer0Expired
//        =0 if no timer overflow has occurred
//        =1,2,3... if 1 overflow, 2 overflows, 3 overflows etc
//        Should be tested as != 0 to exercise and if missed interrupts
//        are important, the count checked.
//

extern int8u ucTimer0Expired;


////////////////////////////////////////////////////////////////////////
// NAME: Timer0Init
//
// PARAMETERS: void (*vpf)(void) - pointer to function to be called
//                                 on expiration (may be NULL)
//
// RETURNS:    nothing
//
// NOTES:      Initializes timer0
//             EA must be set to 1 after calling this function
//
//

void Timer0Init(t_vpf vpf);


#endif // _TIMER0H_
