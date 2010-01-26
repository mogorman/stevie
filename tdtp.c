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

//#define TDTP_STATE_CLOSE 1

int kill = 0;
//void tdtp_init(void)
void tdtp_blink(void);
//static struct tdtp_state tdtp;
static int tdtp_connect(void)
{
  struct uip_conn *conn;
  uip_ipaddr_t ipaddr;

  uip_ipaddr(&ipaddr, 10, 1,10,126);
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
  if(kill) {
    kill = 0;
    uip_close();
    tdtp_blink();
    tdtp_connect();
  }
  if(uip_connected()) { // only fires on connect
    uip_send("hello world\n", 12);
    return;
  }
  if(uip_newdata()) { // fires on new data in socket
    uip_send("good night\n", 11);
    kill = 1;
  }
}



#if 1
void tdtp_blink(void)
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
#endif
