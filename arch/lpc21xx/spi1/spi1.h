#ifndef __SPI1_H__
#define __SPI1_H__

#include "type.h"

/* SPI select pin */ 
#define SPI_SEL      (1 << 11) 

void SPI_Init( void ); 
void SPI_Send( BYTE *Buf, DWORD Length ); 
void SSP_SendRecvByte( BYTE *Buf, DWORD Length ); 
BYTE SSP_SendRecvByteByte( void ); 

#endif
