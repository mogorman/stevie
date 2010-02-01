#include <avr/io.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "serial.h"


#define UART_BAUD_RATE                  57600 /* default is 57600 */ 
#define UART_BAUD_SELECT                (F_CPU / (UART_BAUD_RATE * 16L) - 1)
#define UART_BUFFER_SIZE        16
#define UART0_DATA      UDR0


uint8_t uart_buffer[UART_BUFFER_SIZE];
volatile uint8_t uart_readptr;
volatile uint8_t uart_writeptr;

static int serial_getchar(FILE* stream);
static int serial_putchar(char data, FILE* stream);


void serial_usartrx_isr(void)
{
        uart_buffer[uart_writeptr] = UART0_DATA;
        uart_writeptr = (uart_writeptr + 1) % UART_BUFFER_SIZE;
}

void serial_init(void)
{
        uart_writeptr = 0;
        uart_readptr = 0;

        // set default baud rate 
        UBRR0H = UART_BAUD_SELECT >> 8;
        UBRR0L = UART_BAUD_SELECT;

        // enable receive, transmit and ensable receive interrupts 
        UCSR0B = (1<<RXEN0)|(1<<TXEN0)|(1<<RXCIE0);
	fdevopen(serial_putchar, serial_getchar);
        printf("TTY ON\r\n");
        /* dont forget sei() */
}

static int serial_getchar(FILE* stream)
{
        uint8_t c;

        while( uart_writeptr == uart_readptr); /* block waiting */
        c = uart_buffer[uart_readptr];
        uart_readptr = (uart_readptr + 1) % UART_BUFFER_SIZE;

        return c;
}

static int serial_putchar(char data, FILE* stream)
{
        while ( !(UCSR0A & (1<<UDRE0)) ); /* block till we have space */
        UART0_DATA = data; /* send */
        return data;
}

