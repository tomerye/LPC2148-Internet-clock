#ifndef __UART0_H__
#define __UART0_H__

#include "type.h"
#include "io.h"
//#include "syscalls.h"

#define CLOCKS_PCLK 6000000
#define UART0_BAUD_RATE 19200

///extern const devop_tab_t devop_tab_uart0;

void uart0Init(void);

void uart0SendByte(uint8_t c);

int uart0GetByte(void);
int uart0GetByteWait(void);

#endif
