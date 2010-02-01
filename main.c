#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#include "witypes.h"
#include "network.h"
#include "g2100.h"
#include "spi.h"
#include "stack.h"
#include "config.h"
#include "uip.h"

#ifdef MOG_DEBUG
	#include "serial.h"
#endif

#define WIRELESS_MODE_INFRA	1
#define WIRELESS_MODE_ADHOC	2


// Wireless configuration parameters ----------------------------------------
const prog_char ssid[] PROGMEM = {"topsecret"};		// max 32 bytes

// WPA/WPA2 passphrase
const prog_char security_passphrase[] PROGMEM = {"m*<wX9OUwd"};	// max 64 characters

unsigned char ssid_len;
unsigned char security_passphrase_len;

unsigned char security_type = 3;
unsigned char wireless_mode = WIRELESS_MODE_INFRA;



static void main_init()
{
        tdtp_state = 0;
	tdtp_hw_init();
#ifdef MOG_DEBUG
	serial_init();
#endif
	clock_init();
	zg_init();

//#ifdef USE_DIG0_INTR
//	attachInterrupt(0, zg_isr, LOW);
//#endif
#ifdef USE_DIG0_INTR
	EICRA = (EICRA & ~(_BV(ISC00) | _BV(ISC01))) | (0x0 << ISC00);
	EIMSK |= _BV(INT0);
#endif

#ifdef USE_DIG8_INTR
	// set digital pin 8 on Arduino
	// as ZG interrupt pin
	PCICR |= _BV(PCIE0);
	PCMSK0 |= _BV(PCINT0);
#endif

	while(zg_get_conn_state() != 1) {
		zg_drv_process();
	}
	stack_init();
}

int main (void)
{
	main_init();
	while (1) {
		stack_process();
		zg_drv_process();
                if(tdtp_state) {
                        tdtp_state = 0;
                        tdtp_init();
                }
	}
        return 0;
}

#if defined USE_DIG8_INTR
// PCINT0 interrupt vector
ISR(PCINT0_vect)
{
	zg_isr();
}
#endif
ISR(INT0_vect)
{
	zg_isr();
}

ISR(TIMER0_OVF_vect)
{
	clock_timer0_isr();
	tdtp_timer0_isr();
}

#ifdef MOG_DEBUG
ISR(USART_RX_vect)
{
        serial_usartrx_isr();
}
#endif
