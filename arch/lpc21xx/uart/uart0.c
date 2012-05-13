#include <math.h>
#include <reent.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "uart0.h"
#include "syscalls.h"

uint32_t uart0IsTxBufferEmpty() {
    return 1;
}

void uart0Init() {
    U0LCR = BIT1 | BIT0 /* 8-bit words                            */
        | BIT7;             /* enable programming of divisor latches  */

    {
        uint32_t divaddval, mulval, divisor;
        double exact_rate, exact_divisor;
        double error, min_exact_rate, min_error = 1e300;
        uint32_t min_divaddval, min_mulval, min_divisor;

        for (divaddval=0; divaddval <= 15; divaddval++) {
            for (mulval=1; mulval <= 15; mulval++) {
                exact_divisor = (double) CLOCKS_PCLK / ( (double) UART0_BAUD_RATE
                        * 16.0 * ( 1.0 + ( (double) divaddval / (double) mulval ) ) );
                divisor = round(exact_divisor);
                exact_rate = (double) CLOCKS_PCLK / ( (double) divisor * 16.0 * ( 1.0
                            + ( (double) divaddval / (double) mulval ) ) );
                error = fabs(exact_rate - (double) UART0_BAUD_RATE );
                if (error < min_error) {
                    min_error = error;
                    min_exact_rate = exact_rate;
                    min_divaddval = divaddval;
                    min_mulval = mulval;
                    min_divisor = divisor;
                }
            }
        }
        U0DLM = (min_divisor & 0x0000FF00) >> 8;
        U0DLL = (min_divisor & 0x000000FF);
        U0FDR = min_divaddval | (min_mulval << 4);
    }

    U0LCR &= ~BIT7; /* disable programming of divisor latches */

    U0FCR = BIT0 | BIT1 | BIT2; /* FIFO control: enable & reset    */

    PINSEL0 &= ~(BIT1 | BIT3); /* select UART function               */
    PINSEL0 |= BIT0 | BIT2; /* for pins P0.0 and P0.1             */
}

void uart0SendByte(uint8_t c) {
    while (!(U0LSR & BIT5));
    U0THR = c;
}

//
// Returns: character on success, -1 if no character is available 
int uart0GetByte(void)
{
    // Check if character is available
    if (U0LSR & BIT0) {                
        return U0RBR;
    }

    return -1;
}

//
//  Returns: character on success, waits
int uart0GetByteWait(void)
{
    // Wait for character
    while ( !(U0LSR & BIT0) );

    return U0RBR;              // return character
}

//
// Map driver calls to system calls
//

int uart0_open_r(struct _reent *r, const char *path, int flags, int mode);
int uart0_close_r(struct _reent *r, int fd);
_ssize_t uart0_read_r(struct _reent *r, int fd, void *ptr, size_t len);
_ssize_t uart0_write_r(struct _reent *r, int fd, const void *ptr, size_t len);
_off_t uart0_lseek_r(struct _reent *r, int fd, _off_t ptr, int dir);
int uart0_fstat_r(struct _reent *r, int fd, struct stat *stat_buf);


// Init uart on open, Always succeeds
int uart0_open_r(struct _reent *r, const char *path, int flags, int mode) {
    uart0Init();
    return 0;
}

// Close always succeeds for uart
int uart0_close_r(struct _reent *r, int fd) {
   return 0; 
}

// Code by Alexey Shusharin
_ssize_t uart0_read_r(struct _reent *r, int fd, void *ptr, size_t len)
{
    char c;
    int  i;
    unsigned char *p = (unsigned char*)ptr;

    for (i = 0; i < len; i++)
    {
	c = uart0GetByteWait();

	*p++ = c;
	uart0SendByte(c);

	if (c == 0x0D && i <= (len - 2))
	{
	    *p = 0x0A;
	    uart0SendByte(0x0A);
	    return i + 2;
	}
    }
    return i;
}


_ssize_t uart0_write_r(struct _reent *r, int fd, const void *ptr, size_t len) {
    int i;
    const uint8_t *p = (const uint8_t *) ptr;

    for (i = 0; i < len; i++) {
	if (*p == '\n' ) {
	    uart0SendByte('\r');
	}
	uart0SendByte(*p++);
    }

    return len;
}

_off_t uart0_lseek_r(struct _reent *r, int fd, _off_t ptr, int dir) {
    //  Always indicate we are at file beginning
    return (_off_t)0;
}

int uart0_fstat_r(struct _reent *r, int fd, struct stat *stat_buf) {
    //  Always set as character device.
    stat_buf->st_mode = S_IFCHR;

    /// Assigned to strong type with implicit signed/unsigned conversion.  
    // Required by newlib.
    return 0;
}

// Define the driver operations table
extern const devop_tab_t devop_tab_uart0 = { 
    .name = "uart0",
    .open_r = uart0_open_r,
    .close_r = uart0_close_r,
    .write_r = uart0_write_r,
    .read_r = uart0_read_r,
    .lseek_r = uart0_lseek_r,
    .fstat_r = uart0_fstat_r
};


