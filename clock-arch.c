/* LGPL mostly lifted from arduino source */

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/sfr_defs.h>

#include "clock.h"

#define clockCyclesToMicroseconds(a) ( (a) / clockCyclesPerMicrosecond() )
#define clockCyclesPerMicrosecond() ( F_CPU / 1000000L )


// the prescaler is set so that timer0 ticks every 64 clock cycles, and the
// the overflow handler is called every 256 ticks.
#define MICROSECONDS_PER_TIMER0_OVERFLOW (clockCyclesToMicroseconds(64 * 256))

// the whole number of milliseconds per timer0 overflow
#define MILLIS_INC (MICROSECONDS_PER_TIMER0_OVERFLOW / 1000)

// the fractional number of milliseconds per timer0 overflow. we shift right
// by three to fit these numbers into a byte. (for the clock speeds we care
// about - 8 and 16 MHz - this doesn't lose precision.)
#define FRACT_INC ((MICROSECONDS_PER_TIMER0_OVERFLOW % 1000) >> 3)
#define FRACT_MAX (1000 >> 3)

volatile unsigned long timer0_overflow_count = 0;
volatile unsigned long timer0_millis = 0;
static unsigned char timer0_fract = 0;

SIGNAL(TIMER0_OVF_vect)
{
        // copy these to local variables so they can be stored in registers
        // (volatile variables must be read from memory on every access)
        unsigned long m = timer0_millis;
        unsigned char f = timer0_fract;

        m += MILLIS_INC;
        f += FRACT_INC;
        if (f >= FRACT_MAX) {
                f -= FRACT_MAX;
                m += 1;
        }

        timer0_fract = f;
        timer0_millis = m;
        timer0_overflow_count++;
}

unsigned long millis()
{
        unsigned long m;
        uint8_t oldSREG = SREG;

        // disable interrupts while we read timer0_millis or we might get an
        // inconsistent value (e.g. in the middle of a write to timer0_millis)
        cli();
        m = timer0_millis;
        SREG = oldSREG;

        return m;
}


void clock_init()
{
        // this needs to be called before setup() or some functions won't
        // work there
        sei();

        // on the ATmega168, timer 0 is also used for fast hardware pwm
        // (using phase-correct PWM would mean that timer 0 overflowed half as often
        // resulting in different millis() behavior on the ATmega8 and ATmega168)
#if !defined(__AVR_ATmega8__)
	TCCR0A |= _BV(WGM01);
	TCCR0A |= _BV(WGM00);
#endif
        // set timer 0 prescale factor to 64
#if defined(__AVR_ATmega8__)
	TCCR0 |= _BV(CS01);
	TCCR0 |= _BV(CS00);
#else
	TCCR0B |= _BV(CS01);
	TCCR0B |= _BV(CS00);
#endif
        // enable timer 0 overflow interrupt
#if defined(__AVR_ATmega8__)
	TIMSK |= _BV(TOIE0);
#else
	TIMSK0 |= _BV(TOIE0);
#endif

        // timers 1 and 2 are used for phase-correct hardware pwm
        // this is better for motors as it ensures an even waveform
        // note, however, that fast pwm mode can achieve a frequency of up
        // 8 MHz (with a 16 MHz clock) at 50% duty cycle

        // set timer 1 prescale factor to 64
	TCCR1B |= _BV(CS11);
	TCCR1B |= _BV(CS10);
        // put timer 1 in 8-bit phase correct pwm mode
	TCCR1A |= _BV(WGM10);
        // set timer 2 prescale factor to 64
#if defined(__AVR_ATmega8__)
	TCCR2 |= _BV(CS22);
#else
	TCCR2B |= _BV(CS22);
#endif
        // configure timer 2 for phase correct pwm (8-bit)
#if defined(__AVR_ATmega8__)
	TCCR2 |= _BV(WGM20);
#else
	TCCR2A |= _BV(WGM20);
#endif
#if defined(__AVR_ATmega1280__)
        // set timer 3, 4, 5 prescale factor to 64
	TCCR3B |= _BV(CS31);	TCCR3B |= _BV(CS30);
	TCCR4B |= _BV(CS41);	TCCR4B |= _BV(CS40);
	TCCR5B |= _BV(CS51);	TCCR5B |= _BV(CS50);
        // put timer 3, 4, 5 in 8-bit phase correct pwm mode
	TCCR3A |= _BV(WGM30);
	TCCR4A |= _BV(WGM40);
	TCCR5A |= _BV(WGM50);
#endif

        // set a2d prescale factor to 128
        // 16 MHz / 128 = 125 KHz, inside the desired 50-200 KHz range.
        // XXX: this will not work properly for other clock speeds, and
        // this code should use F_CPU to determine the prescale factor.
	ADCSRA |= _BV(ADPS2);
	ADCSRA |= _BV(ADPS1);
	ADCSRA |= _BV(ADPS0);

        // enable a2d conversions
	ADCSRA |= _BV(ADEN);

        // the bootloader connects pins 0 and 1 to the USART; disconnect them
        // here so they can be used as normal digital i/o; they will be
        // reconnected in Serial.begin()
#if defined(__AVR_ATmega8__)
        UCSRB = 0;
#else
        UCSR0B = 0;
#endif
}

//Return time
clock_time_t clock_time()
{
	return millis();
}
