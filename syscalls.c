//
// System Call Remapping
//

#include <errno.h>
#include <string.h>

#include "syscalls.h"

#define UNUSED_PARAM(p) ((void)(p)) 

// Device drivers used
extern const devop_tab_t devop_tab_uart0;


static devop_tab_t *devop_tab_list[] = {
    &devop_tab_uart0,  /* standard input */
    &devop_tab_uart0,  /* standard output */
    &devop_tab_uart0,  /* standard error */
    0 // End of list
};

int _open_r(struct _reent *preent, const char *file, int flags, int mode)
{
    UNUSED_PARAM(flags);
    UNUSED_PARAM(mode);

    int which_devoptab = 0;
    int fd = -1;

    /* search for "file" in devop_tab_list[].name */
    do {
	int len = strlen(devop_tab_list[which_devoptab]->name);
	if(strncmp(devop_tab_list[which_devoptab]->name, file, len) == 0) {
	    fd = which_devoptab;
	    break;
	}
    } while( devop_tab_list[++which_devoptab] );

    /* if we found the requested file/device,
       then invoke the device's open_r() method */
    if( fd != -1 ) {
	devop_tab_list[fd]->open_r(preent, file, flags, mode);
    }
    else {
	/* it doesn't exist! */
	preent->_errno = ENODEV;
    }

    return fd;
} 

int _close_r (struct _reent *preent, int fd) {
   return devop_tab_list[fd]->close_r(preent, fd);
}

_ssize_t _write_r(struct _reent *preent, int fd, const void *buf, size_t cnt) {
    // Call appropriate driver from list
    return devop_tab_list[fd]->write_r(preent, fd, buf, cnt);
}

_ssize_t _read_r( struct _reent *preent, int fd, void *buf, size_t cnt) {
    // Call appropriate driver from list
    return devop_tab_list[fd]->read_r(preent, fd, buf, cnt);
}

_off_t _lseek_r(struct _reent *preent, int fd, _off_t of, int dir)
{
     // Call appropriate driver from list
    return devop_tab_list[fd]->lseek_r(preent, fd, of, dir);
}

int _fstat_r(struct _reent *preent, int fd, struct stat *stat_buf)
{
     // Call appropriate driver from list
    return devop_tab_list[fd]->fstat_r(preent, fd, stat_buf);
}

// Avoid warning
int isatty(int fd); 
int isatty(int fd) {
    return 1;
}


// end is set in the linker command file and 
// is the end of statically allocated data (thus start of heap).

#ifdef HEAP_SIZE

#define HEAP_OVERFLOW_MSG	    "Panic: Heap Overflow !\n"
#define HEAP_OVERFLOW_MSG_LEN   24
extern char end[HEAP_SIZE];

#else
extern char end[];
#endif

// Points to current end of the heap
static char *pheap_end;	

//
// Adjusts end of heap to provide more memory to memory allocator. 
//
//  struct _reent *r - re-entrancy structure, used by newlib to
//		       support multiple threads of operation.
//  ptrdiff_t nbytes - number of bytes to add.
//
//  Returns pointer to start of new heap area.
//
//  Note: This implementation is not thread safe 
//        (despite taking a _reent structure as a parameter).
//
void * _sbrk_r(struct _reent *preent, ptrdiff_t nbytes) {
    UNUSED_PARAM(preent);

    // errno should be set to  ENOMEM on error
    char *base;		

    // First time init
    if (!pheap_end) {	
	pheap_end = end;
    }

    // Save heap end
    base = pheap_end;	

#ifdef HEAP_SIZE
    // Check overflow
    if( pheap_end + incr - end > HEAP_SIZE ) {

	// Heap Overflow — announce on stderr
	write(2, HEAP_OVERFLOW_MSG, HEAP_OVERFLOW_MSG_LEN);
	abort();
    }
#endif

    // Increase
    pheap_end += nbytes;

    // Return pointer to start of new heap area.
    return base;		
}


