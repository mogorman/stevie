#include <stdio.h>
#include <string.h>
#include <util/delay.h>
#include <stdlib.h>

#include "tdtp.h"
#include "uip.h"
#include "timer.h"
#include "pt.h"
#include "psock.h"
//PB0 = port 8
//PB1 = port 9 head
//PB2 = dong and port 10?

//#define TDTP_STATE_CLOSE 1

/* controls exported */
/* arg1 = motor						options being 1,2,3 */
/* arg2 = direction					options being 0,1 */
/* arg3 = intensity					options being 0-255 */
/* arg4 = length of time in ms, with 0 meaning forever	options being 0-20,000 */

/* <control vibe="rabbit" version="1.0"> */
/* 	 <argument type="motor" option */
/* </control> */

/* initial packet contains  */
/* Product_id:version_code:mac */

/* state packet matches input packet */

/* command packet gets acked immediately that state was changed */


/* 1,0,255,20000 */



static int handle_tdtp_connection(struct tdtp_state *t);
//static void tdtp_blink(void);


//
void tdtp_timer0_isr(void)
{
}

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

void tdtp_hw_init(void)
{
	DDRB = _BV(PB1);
}

void tdtp_init(void)
{
        tdtp_connect();
}

void tdtp_appcall(void)
{
        struct tdtp_state *t = &(uip_conn->appstate);
        PSOCK_INIT(&t->p, (uint8_t *)t->inputbuffer, sizeof(t->inputbuffer));
        handle_tdtp_connection(t);
}

static int handle_tdtp_connection(struct tdtp_state *t)
{
	int result = 0;
        char *p, *tmp, *to_free;
        int new_motor = 0, new_direction = 0, new_pwm = 0, new_time = 0;
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
                tmp = to_free = strdup(t->inputbuffer);
                if(!tmp)
                        return 1;
                p = strsep(&tmp, ",");
                if(p)
                        new_motor = atoi(p);
                else
                        goto yuck;
                p = strsep(&tmp, ",");
                if(p)
                        new_direction = atoi(p);
                else
                        goto yuck;
                p = strsep(&tmp, ",");
                if(p)
                        new_pwm = atoi(p);
                else
                        goto yuck;
                p = strsep(&tmp, ",");
                if(p)
                        new_time = atoi(p);
                else {
                yuck:
                        if(to_free)
                                free(to_free);
                        return 1;
                }
                if(to_free)
                        free(to_free);

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


//
//static void tdtp_blink(void)
//{
//  /* char i; */
//  /* printf("hello you\r\n"); */
//  /* DDRB = _BV(PB0); // make Arduino Pin 13 (Atmega8 PortB bit 5) an output (%0000100) */
//  /*   for(i = 0; i < 10; i++){ */
//  /*     _delay_ms(30);      // max is 262.14 ms / F_CPU in MHz */
//  /*   } */
//  /*   PORTB ^= _BV(PB0);     // put Arduino Pin 13 (Atmega8 PortB bit 5) HIGH (%00100000) */
//  /*   for(i = 0; i < 10; i++){ */
//  /*     _delay_ms(30); */
//  /*   } */
//    PORTB ^= _BV(PB1);     // put Arduino Pin 13 (Atmega8 PortB bit 5) LOW (%00000000)
//}
