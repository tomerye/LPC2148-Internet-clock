#ifndef __LCD_H__
#define __LCD_H__

#include "type.h"
#include "lpc214x.h"
#include "io.h"

/* For lcdGoto */
#define LEFT  0x0
#define RIGHT 0x4
#define CURSOR_DISP_SHIFT 0x10

#define lcd_backlight_on() { IODIR0 |= BIT30; \
	IOSET0 = BIT30; }
#define lcd_backlight_off() { IODIR0 &= ~BIT30; \
	IOCLR0 = BIT30; }

void lcdPrintChar(char c);

void lcdGoto(int dest_addr);

void lcdPrintString(char *str);

void set_bitmode(int mode);

void lcdInit();

void lcdClearScreen();

void busywait(uint64_t microseconds);

#endif

