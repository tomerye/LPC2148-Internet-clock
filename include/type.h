#ifndef __TYPE_H__
#define __TYPE_H__

#include <stdint.h>

#ifndef NULL
        #define NULL ((void *)0)
#endif

#ifndef FALSE
        #define FALSE (0)
#endif

#ifndef TRUE
        #define TRUE (1)
#endif

/* These types are assumed as 16-bit or larger integer */
typedef signed int	INT;
typedef unsigned int	UINT;

/* These types are assumed as 8-bit integer */
typedef signed char	CHAR;
typedef unsigned char	UCHAR;
typedef unsigned char	BYTE;

/* These types are assumed as 16-bit integer */
typedef signed short	SHORT;
typedef unsigned short	USHORT;
typedef unsigned short	WORD;

/* These types are assumed as 32-bit integer */
typedef signed long	LONG;
typedef unsigned long	ULONG;
typedef unsigned long	DWORD;

typedef unsigned char BOOL;
typedef unsigned char bool;



typedef    unsigned  char char_t;
/*
typedef      signed  char          int8_t;
typedef    unsigned  char         uint8_t;

typedef      signed  short        int16_t;
typedef    unsigned  short       uint16_t;

typedef      signed  int          int32_t;
typedef    unsigned  int         uint32_t;

typedef      signed  long long    int64_t;
typedef    unsigned  long long   uint64_t;
*/
typedef      signed  char   *     int8_ptr;
typedef    unsigned  char   *     uint8_ptr;

typedef      signed  short  *      int16_ptr;
typedef    unsigned  short  *     uint16_ptr;

typedef      signed  int    *      int32_ptr;
typedef    unsigned  int    *    uint32_ptr;

typedef      signed  long long  *   int64_ptr;
typedef    unsigned  long long  *  uint64_ptr;


typedef    volatile  signed  char   *     vint8_ptr;
typedef    volatile unsigned  char  *     vuint8_ptr;

typedef    volatile  signed  short  *      vint16_ptr;
typedef    volatile unsigned  short *     vuint16_ptr;

typedef    volatile  signed  int    *      vint32_ptr;
typedef    volatile unsigned  int   *    vuint32_ptr;

typedef    volatile  signed  long long   *   vint64_ptr;
typedef    volatile unsigned  long long  *  vunit64_ptr;

#define	LD_WORD_BE(ptr)	        \
    (WORD) (((WORD)*(volatile BYTE*)((ptr)+0)<<8)|(WORD)*(volatile BYTE*)((ptr)+1))

#define	LD_DWORD_BE(ptr)	\
    (DWORD)(((DWORD)*(volatile BYTE*)((ptr)+0)<<24)|((DWORD)*(volatile BYTE*)((ptr)+1)<<16)|((WORD)*(volatile BYTE*)((ptr)+2)<<8)|*(volatile BYTE*)((ptr)+3))

#define	ST_WORD_BE(ptr,val)	\
    *(volatile BYTE*)((ptr)+1)=(BYTE)(val); *(volatile BYTE*)((ptr))=(BYTE)((WORD)(val)>>8)

#define	ST_DWORD_BE(ptr,val)	                            \
    *(volatile BYTE*)((ptr)+3)=(BYTE)(val);                 \
    *(volatile BYTE*)((ptr)+2)=(BYTE)((WORD)(val)>>8);      \
    *(volatile BYTE*)((ptr)+1)=(BYTE)((DWORD)(val)>>16);    \
    *(volatile BYTE*)((ptr)+0)=(BYTE)((DWORD)(val)>>24)

typedef union
{
	unsigned char byte[2];
	unsigned short int number;
} uint16_u;

typedef union
{
	unsigned char byte[4];
	unsigned  int number;
} uint32_u;


#endif /* __TYPE_H__ */

