#include <stdio.h>
#include <string.h>
#include <util/delay.h>

#include "tdtp.h"
#include "uip.h"
#include "timer.h"
#include "pt.h"
#include "psock.h"
//PB0 = port 8
//PB1 = port 9 head
//PB2 = dong and port 10?

//#define TDTP_STATE_CLOSE 1


static void tdtp_blink(void);
static int handle_tdtp_connection(struct tdtp_state *t);

static int tdtp_connect(void)
{
        struct uip_conn *conn;
        uip_ipaddr_t ipaddr;

        uip_ipaddr(&ipaddr, 10, 69, 69, 1);
        conn = uip_connect(&ipaddr, htons(6969));
        if(conn == NULL) {
#ifdef MOG_DEBUG
                printf("failure out of ports\n");
#endif
                return 0;
        }
        return 1;
}

void tdtp_init(void)
{
        tdtp_connect();
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
        if(uip_connected()) {
#ifdef MOG_DEBUG
                printf("failure closed\n");
#endif
                uip_send("hello connected\n", 16);
        }
	if(uip_newdata()) {
                memset(&t->inputbuffer, 0, sizeof(t->inputbuffer));
		PSOCK_READTO(&t->p, '\n');
	#ifdef MOG_DEBUG
		printf("hello %d:%s:\n", uip_datalen(), t->inputbuffer);
	#endif
		uip_send("hi2\n",4);
	} else if(uip_closed()) {
#ifdef MOG_DEBUG
                printf("failure closed\n");
#endif
		uip_close();
		while(!tdtp_connect()) {
			_delay_ms(30);
		}
		result=1;
	} else if(uip_aborted() || uip_timedout()) {
#ifdef MOG_DEBUG
                printf("failure aborted\n");
#endif
		uip_abort();
		while(!tdtp_connect()) {
			_delay_ms(30);
		}
		result=1;
	}
	PSOCK_END(&t->p);
	return result;
}



static void tdtp_blink(void)
{
  /* char i; */
  /* printf("hello you\r\n"); */
  /* DDRB = (1<<PB0); // make Arduino Pin 13 (Atmega8 PortB bit 5) an output (%0000100) */
  /*   for(i = 0; i < 10; i++){ */
  /*     _delay_ms(30);      // max is 262.14 ms / F_CPU in MHz */
  /*   } */
  /*   PORTB ^= (1<<PB0);     // put Arduino Pin 13 (Atmega8 PortB bit 5) HIGH (%00100000) */
  /*   for(i = 0; i < 10; i++){ */
  /*     _delay_ms(30); */
  /*   } */
    PORTB ^= (1<<PB1);     // put Arduino Pin 13 (Atmega8 PortB bit 5) LOW (%00000000)
}
