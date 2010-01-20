//#define UART_BUFFER_SIZE        16
//#define UART0_DATA      UDR0
//#define UART0_ISR_VECT USART_RX_vect
//#define UART_BAUD_RATE                  9600 /* default is 57600 */ 
//#define UART_BAUD_SELECT                (F_CPU / (UART_BAUD_RATE * 16L) - 1)
//
//uint8_t uart_buffer[UART_BUFFER_SIZE];
////volatile uint8_t uart_readptr;
//volatile uint8_t uart_writeptr;
//
///* to use serial functions, do init then use printf() and getchar() */
void serial_init(void);
int serial_putchar(char data, FILE* stream);


