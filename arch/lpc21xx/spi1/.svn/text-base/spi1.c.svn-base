#include "spi1.h"
#include "io.h"

/* 
 *  SPI and MMC commands related modules. 
 *   
 */ 
void SPI_Init( void ) 
{ 
  DWORD portConfig; 
  BYTE i, Dummy; 

  /* Configure PIN connect block */ 
  /* bit 32, 54, 76 are 0x10, bit 98 are 0x00 
  port 0 bits 17, 18, 19, 20 are SSP port SCK1, MISO1, MOSI1,  
  and SSEL1 set SSEL to GPIO pin that you will have the totoal  
  freedom to set/reset the SPI chip-select pin */ 
    
  SSPCR1 = 0x00; /* SSP master (off) in normal mode */ 
 
  portConfig = PINSEL1; 
  PINSEL1 = portConfig | 0x00A8; 
  IODIR0 |= SPI_SEL;  /* SSEL is output */ 
  IOSET0 = SPI_SEL;  /* set SSEL to high */ 
      /* Set PCLK 1/2 of CCLK */ 
  VPBDIV = 0x02; 
 
  /* Set data to 8-bit, Frame format SPI, CPOL = 0, CPHA = 0,  
  and SCR is 7 */ 
  SSPCR0 = 0x0707; 
 
  /* SSPCPSR clock prescale register, master mode, minimum divisor  
  is 0x02*/ 
  SSPCPSR = 0x2; 
 
  /* Device select as master, SSP Enabled, normal operational mode */ 
  SSPCR1 = 0x02; 
 
  for ( i = 0; i < 8; i++ ) 
  { 
    Dummy = SSPDR;    /* clear the RxFIFO */ 
  } 
print("spi init OK\n"); 
 return; 
 
} 
 
/*  
 * SPI Send a block of data based on the length 
 */ 
void SPI_Send( BYTE *buf, DWORD Length ) 
{ 
  BYTE Dummy; 
 
  if ( Length == 0 ) 
   return; 
  while ( Length != 0 ) 
  { 
    /* as long as TNF bit is set, TxFIFO is not full, I can write */  
    while ( !(SSPSR & 0x02) ); 
    SSPDR = *buf; 
    /* Wait until the Busy bit is cleared */ 
    while ( !(SSPSR & 0x04) ); 
    Dummy = SSPDR;        /* Flush the RxFIFO */ 
   Length--; 
   buf++; 
  } 
  return;  
} 
 
/*  
 * SPI receives a block of data based on the length 
 */ 
void SSP_SendRecvByte( BYTE *buf, DWORD Length ) 
{ 
  DWORD i; 
 
  for ( i = 0; i < Length; i++ ) 
  { 
    *buf = SSP_SendRecvByteByte(); 
   buf++; 
  } 
  return;  
} 
 
/*  
 * SPI Receive Byte, receive one byte only, return Data byte 
 * used a lot to check the status. 
 */ 
BYTE SSP_SendRecvByteByte( void ) 
{ 
  BYTE data; 
 
  /* wrtie dummy byte out to generate clock, then read data from  
  MISO */ 
  SSPDR = 0xFF; 
  /* Wait until the Busy bit is cleared */ 
  while ( SSPSR & 0x10 ); 
  data = SSPDR; 
  //print ("Data recv =");
  //printNum(data);
  //print("\n");
  
  return ( data );  
} 

