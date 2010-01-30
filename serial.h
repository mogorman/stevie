#ifndef __SERIAL_H__
#define __SERIAL_H__

void serial_init(void);
int serial_putchar(char data, FILE* stream);

#endif
