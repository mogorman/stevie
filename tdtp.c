#include <stdio.h>
#include <string.h>
#include <util/delay.h>

#include "tdtp.h"
#include "uip.h"
#include "timer.h"
#include "pt.h"

//PB0 = port 8
//PB1 = port 9 head
//PB2 = dong and port 10?

//static struct tdtp_state t;
static uip_ipaddr_t tdtp_ipaddr;
static int handle_tdtp_connection(struct tdtp_state *t);

int connected(void)
{
	struct uip_conn *conn = NULL;
	conn = uip_connect(&tdtp_ipaddr, HTONS(6969));
	if(conn == NULL) {
		_delay_ms(30);
		return 0;
	}
	return 1;
}

void tdtp_init(void)
{
//	unsigned char host_tdtp[] = {10,1,10,126};
	unsigned char host_tdtp[] = {192,168,1,144};
	uip_ipaddr_copy(&tdtp_ipaddr, &host_tdtp);
	while(!connected());
	uip_send("hi1\n", 4);
}

void tdtp_appcall(void)
{
	struct tdtp_state *t = &(uip_conn->appstate);
	PSOCK_INIT(&t->p, t->inputbuffer, sizeof(t->inputbuffer));
	handle_tdtp_connection(t);

}

static int handle_tdtp_connection(struct tdtp_state *t)
{
	int result = 0;
	PSOCK_BEGIN(&t->p);
	if(uip_newdata()) {
		PSOCK_READTO(&t->p, '\n');
	#ifdef MOG_DEBUG
		printf("hello %d:%s:\n", uip_datalen(), t->inputbuffer);
	#endif
		uip_send("hi2\n",4);
	}
	if(uip_closed()) {
		uip_unlisten(6969);
		while(!connected()) {
			_delay_ms(30);
		}
		PSOCK_INIT(&t->p, t->inputbuffer, sizeof(t->inputbuffer));
		uip_send("hi3\n", 4);
		result=1;
	} else if(uip_aborted() || uip_timedout()) {
		uip_unlisten(6969);
		while(!connected()) {
			_delay_ms(30);
		}
		PSOCK_INIT(&t->p, t->inputbuffer, sizeof(t->inputbuffer));
		uip_send("hi4\n", 4);
		result=1;
	}
	PSOCK_END(&t->p);
	return result;
}

#if 0
void blink(void)
{
	char i;
	printf("hello you\r\n");
	DDRB = (1<<PB0); // make Arduino Pin 13 (Atmega8 PortB bit 5) an output (%0000100)
	for(;;){
		for(i = 0; i < 10; i++){
			_delay_ms(30);      // max is 262.14 ms / F_CPU in MHz
		}
		PORTB ^= (1<<PB0);     // put Arduino Pin 13 (Atmega8 PortB bit 5) HIGH (%00100000)
		for(i = 0; i < 10; i++){
			_delay_ms(30);
		}
		PORTB ^= (1<<PB0);     // put Arduino Pin 13 (Atmega8 PortB bit 5) LOW (%00000000)
	}
}
#endif
