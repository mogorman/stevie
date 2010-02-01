#ifndef __TDTP_H__
#define __TDTP_H__

#include "uipopt.h"
#include "psock.h"


struct tdtp_state {
        char inputbuffer[15];
        struct psock p;
};

typedef struct tdtp_state uip_tcp_appstate_t;

int tdtp_state;
void tdtp_hw_init(void);
void tdtp_init(void);
void tdtp_appcall(void);
void tdtp_timer0_isr(void);

#ifndef UIP_APPCALL
#define UIP_APPCALL     tdtp_appcall
#endif

#endif
