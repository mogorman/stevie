#include <avr/io.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "serial.h"


#define FOSC 16000000
#define BAUD 9600
#define MYUBRR ((FOSC/16)/BAUD-1)
#define sbi(var, mask)   ((var) |= (uint8_t)(1 << mask))
#define cbi(var, mask)   ((var) &= (uint8_t)~(1 << mask))

#define STATUS_LED 0

//Define functions
//======================

static FILE mystdout = FDEV_SETUP_STREAM(serial_putchar, NULL, _FDEV_SETUP_WRITE);

//======================

void serial_init (void)
{
    //1 = output, 0 = input
    DDRB = 0b11101111; //PB4 = MISO 
    DDRC = 0b11111111; //
    DDRD = 0b11111110; //PORTD (RX on PD0)
    //USART Baud rate: 9600
    UBRR0H = MYUBRR >> 8;
    UBRR0L = MYUBRR;
    UCSR0B = (1<<RXEN0)|(1<<TXEN0);
    
    stdout = &mystdout; //Required for printf init
}

int serial_putchar(char c, FILE *stream)
{
    if (c == '\n') serial_putchar('\r', stream);
  
    loop_until_bit_is_set(UCSR0A, UDRE0);
    UDR0 = c;
    
    return 0;
}

