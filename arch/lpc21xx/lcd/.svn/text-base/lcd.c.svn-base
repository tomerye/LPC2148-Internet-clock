#include "lcd.h"
#include <string.h>

#define max(a,b) ((((int)a)>((int)b)) ? (a) : (b))

#define DB7 BIT23
#define DB6 BIT22
#define DB5 BIT21
#define DB4 BIT20
#define DB3 BIT19
#define DB2 BIT18
#define DB1 BIT17
#define DB0 BIT16

#define ALL	(DB0|DB1|DB2|DB3|DB4|DB5|DB6|DB7)
#define ALL4    (DB4|DB5|DB6|DB7)

#define RW  BIT22
#define RS  BIT24
#define E	BIT25
#define lcd_en_rs_out() ( IODIR1 |= E | RS )

#define lcd_rw_out() (IODIR0 |= RW )

#define lcd_data8_out() ( IODIR1 |= ALL )
#define lcd_data8_in()  ( IODIR1 &=  ~ALL )

#define lcd_data4_out() ( IODIR1 |= ALL4 )
#define lcd_data4_in() ( IODIR1 &= ~ALL4 )

#define lcd_data8_read() \
	((IOPIN1 & ALL))

#define lcd_data4_read() \
	((IOPIN1 & ALL4))

#define lcd_data8_set(c) \
    { IOCLR1 = ALL; IOSET1 = ((uint32_t)c)<<16; }

#define lcd_data4_set(c) \
    { IOCLR1 = ALL; IOSET1 = ((((uint32_t)c)<<16) & ALL4); }


#define lcd_rs_set() (IOSET1 = RS)
#define lcd_rs_clear() (IOCLR1 = RS)

#define lcd_en_set() (IOSET1 = E)
#define lcd_en_clear() (IOCLR1 = E)

#define lcd_rw_set() (IOSET0 = RW)
#define lcd_rw_clear() (IOCLR0 = RW)

void lcdbusywait(uint64_t microseconds)
{
	volatile uint64_t i = 0;
	uint64_t endLoop = microseconds * 60;

	while ( i < endLoop)
	{
		i++;
	}
}

volatile int g_is8bit = 1;

/*
 * Reads both busy bit and address counter
 */
void read8(int *busy, int *address) {

	lcd_rs_clear();
	lcd_rw_set();
	lcd_data8_in();

	/* Clock setup time */
	lcdbusywait(1);

	lcd_en_set();

	/* Data output delay time */
	lcdbusywait(2);

	int bits = lcd_data8_read();
	*busy = (bits & DB7) != 0;
	*address = ((bits & ALL & ~DB7) >> 16);

	/* Clock pulse width */
	lcdbusywait(1);

	lcd_en_clear();

	/* Data hold time */
	lcdbusywait(1);

	/* Cycle time */
	lcdbusywait(1);
}

void read4(int *busy, int *address) 
{

	lcd_rs_clear();
	lcd_rw_set();
	lcd_data4_in();

	/* Clock setup time */
	lcdbusywait(1);

	lcd_en_set();

	/* Data output delay time */
	lcdbusywait(2);

	int bits = lcd_data4_read();
	*busy = (bits & DB7) != 0;
	*address = ((bits & ALL4 & ~DB7) >> 16);

	/* Clock pulse width */
	lcdbusywait(1);

	lcd_en_clear();

	/* Data hold time */
	lcdbusywait(1);

	lcd_en_set();

	/* Data output delay time */
	lcdbusywait(2);

	bits = lcd_data4_read();
	*address = ((bits & ALL4) >> 20);

	/* Clock pulse width */
	lcdbusywait(1);

	lcd_en_clear();

	/* Data hold time */
	lcdbusywait(1);

	/* Cycle time */
	lcdbusywait(1);	
}

void pollBusyBit()
{
	int busy, addr;

	while (1) {
		if(g_is8bit) {
			read8(&busy, &addr);
		}
		else {
			read4(&busy, &addr);
		}

		if (busy == 0)
			break;
	}
}

void generic_set8(int bits) 
{
	lcd_data8_out();

	/* Clock setup time */
	lcdbusywait(1);

	lcd_data8_set(bits);
	lcd_en_set();

	/* Clock pulse width */
	lcdbusywait(1);

	lcd_en_clear();

	/* Data hold time */
	lcdbusywait(1);

	/* Cycle time */
	lcdbusywait(1);

	pollBusyBit();
}

void generic_set4(int bits)
{
	lcd_data4_out();

	/* Clock setup time */
	lcdbusywait(1);

	lcd_data4_set(bits);
	lcd_en_set();

	/* Clock pulse width */
	lcdbusywait(1);

	lcd_en_clear();

	/* Data Hold time */
	lcdbusywait(1);

	lcd_data4_set(bits << 4);
	lcd_en_set();

	/* Clock pulse width */
	lcdbusywait(1);

	lcd_en_clear();

	/* Data Hold time */
	lcdbusywait(1);

	/* Cycle time */
	lcdbusywait(1);

	pollBusyBit();
}

void inst(int bits) {

	lcd_rs_clear();
	lcd_rw_clear();
	if(g_is8bit) {
		generic_set8(bits);
	}
	else {
		generic_set4(bits);
	}
}

void write(int bits) {

	lcd_rs_set();
	lcd_rw_clear();
	if(g_is8bit) {
		generic_set8(bits);
	}
	else {
		generic_set4(bits);
	}
}

/* LCD API */

void lcdPrintChar(char c) {
	write(c);
}

void lcdGoto(int dest_addr) {
	int busy;
	int addr;
	int delta;
	int direction = RIGHT;

	if (g_is8bit) {
		read8(&busy, &addr);
	}
	else {
		read4(&busy, &addr);
	}

	delta = dest_addr - addr;

	if (delta < 0)
	{
		delta = -delta;
		direction = LEFT;
	}

	for(int i = 0; i < delta; i++) 
	{
		inst(CURSOR_DISP_SHIFT | direction);
	}
}

void lcdPrintString(char *str) 
{
	int tok = strlen(str);
	for (int i = 0; i < strlen(str); ++i)
	{
		char c = str[i];
		if (c == '\n') {
			inst(0xC0);
			tok = i;
		}
		else {
			lcdPrintChar(c);
		}
	}

	if ((tok > 16) || (strlen(str) - tok > 16))
	{
		lcdbusywait(10000);
		for (int j = 0; j < max(strlen(str) - tok - 16, tok - 16); j++)
		{
			inst(0x18);
			lcdbusywait(1000);
		}

	}
}

void set_bitmode(int mode) 
{
	if (mode == 4) {
		inst(0x28);
	}
	else if (mode == 8){
		inst(0x38);
	}
	else {
		return;
	}

	g_is8bit = (mode == 8);
}

void lcdClearScreen()
{
	inst(0x01); 
}

void lcdInit() 
{
	lcd_en_rs_out();
	lcd_data8_out();
	lcd_rw_out(); 
	lcd_backlight_on(); 

	/* Function Set 1 line  */
	inst(0x38);

	/* Display on, Blink */
	inst(0x0F);

	/* Clear Screen*/ 
	lcdClearScreen();

	/* Increment */
	inst(0x06);

	return;
}


