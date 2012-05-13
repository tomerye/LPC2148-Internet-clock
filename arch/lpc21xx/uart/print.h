#ifndef __PRINT_H__

#include <stdint.h>
#include "io.h"
#include "uart0.h"

#define  print_char(x) uart0SendByte(x)

void print(uint8_t* s);

void printHex(uint32_t v, uint8_t digits);

void printNum(int32_t v);

#endif
