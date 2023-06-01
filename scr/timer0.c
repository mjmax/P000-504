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
#include "avrdet.h"
#include "types.h"
#include "timer0.h"

#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <stdint.h>


/* Timer counter 0 clock sources */
#define TC0_CLK_SRC_NONE        (0 << CS02 | 0 << CS01 | 0 << CS00)
#define TC0_CLK_SRC_DIV1        (0 << CS02 | 0 << CS01 | 1 << CS00)
#define TC0_CLK_SRC_DIV8        (0 << CS02 | 1 << CS01 | 0 << CS00)
#define TC0_CLK_SRC_DIV64       (0 << CS02 | 1 << CS01 | 1 << CS00)
#define TC0_CLK_SRC_DIV256      (1 << CS02 | 0 << CS01 | 0 << CS00)
#define TC0_CLK_SRC_DIV1024     (1 << CS02 | 0 << CS01 | 1 << CS00)
#define TC0_CLK_SRC_EXT_FALLING (1 << CS02 | 1 << CS01 | 0 << CS00)
#define TC0_CLK_SRC_EXT_RISING  (1 << CS02 | 1 << CS01 | 1 << CS00)
#define TC0_CLK_SRC_MASK        (1 << CS02 | 1 << CS01 | 1 << CS00)


#define NUM_TC0_DIVIDERS 5

static const uint8_t tc0_divider_masks[NUM_TC0_DIVIDERS] PROGMEM =
{
  TC0_CLK_SRC_DIV1, TC0_CLK_SRC_DIV8, TC0_CLK_SRC_DIV64, TC0_CLK_SRC_DIV256, TC0_CLK_SRC_DIV1024
};

static const uint16_t tc0_divider_value[NUM_TC0_DIVIDERS] PROGMEM =
{
  1, 8, 64, 256, 1024
};


static t_vpf t0func = (t_vpf)NULL;

int16u ucTimer0Expired;

/** The calculated reload value to be loaded into TCNT0 */
static uint8_t tc0_reload;


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
void Timer0Init(t_vpf vpf)
{
  uint8_t prescaler;
  uint32_t ocr;

  /* Disable timer 0 interrupt */
  TIMSK0 &= ~(1 << TOIE0);


  t0func = vpf;
  ucTimer0Expired = 0;

  for (prescaler = 0; prescaler < NUM_TC0_DIVIDERS; prescaler++)
  {
    ocr = F_CPU / T0_TICK_FREQUENCY / pgm_read_word(&tc0_divider_value[prescaler]);
    if (ocr < UINT8_MAX)
      break;
  }
  if (prescaler < NUM_TC0_DIVIDERS)
  {
    /* Load timer, +1 because it's meant to be 256 - ocr */
    tc0_reload = UINT8_MAX - ocr;
    tc0_reload++;
    TCNT0 = tc0_reload;

    /* Normal count up mode, no compare match, prescaler calculated above */
    TCCR0A = 0 << COM0A1 | 0 << COM0A0 | 0 << COM0B1 | 0 << COM0B0 | 0 << WGM01 | 0 << WGM00;
    TCCR0B = 0 << FOC0A | 0 << FOC0B | 0 << WGM02 | (pgm_read_byte(&tc0_divider_masks[prescaler]) & TC0_CLK_SRC_MASK);
  }

  /* Enable timer 0 interrupt */
  TIMSK0 |= (1 << TOIE0);
}

int16u millis(void)
{
  return ucTimer0Expired;
}


////////////////////////////////////////////////////////////////////////
// NAME: timer0_isr
//
// PARAMETERS: none
//
// RETURNS:    nothing
//
// NOTES:      Timer0 Interrupt Service Routine
//             Timer0Init must be called first
//             Global interrupts are disabled with SIGNAL
//

ISR(TIMER0_OVF_vect)
{
  /* reload the counter */
  TCNT0 = tc0_reload;

  /* do things */
  ucTimer0Expired++;
  if (t0func != (t_vpf)NULL)
  {
    t0func();
  }
}

