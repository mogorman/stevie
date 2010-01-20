
/******************************************************************************

  Filename:		WiShield.cpp
  Description:	WiShield library file for the WiShield 1.0

 ******************************************************************************

  TCP/IP stack and driver for the WiShield 1.0 wireless devices

  Copyright(c) 2009 Async Labs Inc. All rights reserved.

  This program is free software; you can redistribute it and/or modify it
  under the terms of version 2 of the GNU General Public License as
  published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
  more details.

  You should have received a copy of the GNU General Public License along with
  this program; if not, write to the Free Software Foundation, Inc., 59
  Temple Place - Suite 330, Boston, MA  02111-1307, USA.

  Contact Information:
  <asynclabs@asynclabs.com>

   Author               Date        Comment
  ---------------------------------------------------------------
   AsyncLabs			05/01/2009	Initial version
   AsyncLabs			05/29/2009	Adding support for new library

 *****************************************************************************/
#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#include "witypes.h"
#include "global-conf.h"
#include "network.h"
#include "g2100.h"
#include "spi.h"
#include "stack.h"
#include "config.h"
#include "serial.h"

#define WIRELESS_MODE_INFRA	1
#define WIRELESS_MODE_ADHOC	2

// Wireless configuration parameters ----------------------------------------
unsigned char local_ip[] = {0,0,0,0};	// IP address of WiShield
unsigned char gateway_ip[] = {0,0,0,0};	// router or gateway IP address
unsigned char subnet_mask[] = {0,0,0,0};	// subnet mask for the local network
const prog_char ssid[] PROGMEM = {"dd-wrt"};		// max 32 bytes

unsigned char security_type = 0;	// 0 - open; 1 - WEP; 2 - WPA; 3 - WPA2

// WPA/WPA2 passphrase
const prog_char security_passphrase[] PROGMEM = {};	// max 64 characters

// WEP 128-bit keys
// sample HEX keys
prog_uchar wep_keys[] PROGMEM = {};

// setup the wireless mode
// infrastructure - connect to AP
// adhoc - connect to another WiFi device
unsigned char wireless_mode = WIRELESS_MODE_INFRA;

unsigned char ssid_len;
unsigned char security_passphrase_len;

const prog_char webpage[] PROGMEM = {"HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<center><h1>I AM A DILDO SERVING YOU WEBERNETS</h1></center>"};


static void main_init()
{

	clock_init();
#ifdef MOG_DEBUG
	serial_init();
#endif
	zg_init();

//#ifdef USE_DIG0_INTR
//	attachInterrupt(0, zg_isr, LOW);
//#endif
#ifdef USE_DIG0_INTR
	EICRA = (EICRA & ~((1 << ISC00) | (1 << ISC01))) | (0x0 << ISC00);
	EIMSK |= (1 << INT0);
#endif

#ifdef USE_DIG8_INTR
	// set digital pin 8 on Arduino
	// as ZG interrupt pin
	PCICR |= (1<<PCIE0);
	PCMSK0 |= (1<<PCINT0);
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
	}
	return 0;
}

#if defined USE_DIG8_INTR && !defined APP_WISERVER
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

