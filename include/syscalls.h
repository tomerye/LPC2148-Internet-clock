#ifndef __SYSCALLS_H__
#define __SYSCALLS_H__

#include <stdlib.h>
#include <reent.h>
#include <sys/stat.h>
#include <sys/types.h>

typedef struct {
    const char *name;

    int  (*open_r)( struct _reent *r, const char *path, int flags, int mode);

    int  (*close_r)( struct _reent *r, int fd);

    _ssize_t (*write_r)( struct _reent *r, int fd, const void *ptr, size_t cnt);

    _ssize_t (*read_r)( struct _reent *r, int fd, void *ptr, size_t cnt);

    _off_t (*lseek_r)( struct _reent *r, int fd, _off_t ptr, int dir);

    int (*fstat_r)( struct _reent *r, int fd, struct stat *stat_buf);
} devop_tab_t;

#endif
