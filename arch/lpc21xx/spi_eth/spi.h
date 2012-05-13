//
//  $Id: spi.h 331 2008-11-09 16:59:47Z jcw $
//  $Revision: 331 $
//  $Author: jcw $
//  $Date: 2008-11-09 11:59:47 -0500 (Sun, 09 Nov 2008) $
//  $HeadURL: http://tinymicros.com/svn_public/arm/lpc2148_demo/trunk/spi/spi.h $
//

#ifndef _SPI_H_
#define _SPI_H_

#include "type.h"

//
//
//
void spiInit (void);
uint8_t spiPut (uint8_t valueIn);

#endif
