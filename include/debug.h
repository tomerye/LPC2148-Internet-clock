#ifndef __DEBUG_H__
#define __DEBUG_H__

#include <stdarg.h>

#define MSG_CRIT        0
#define MSG_WARN        15
#define MSG_INFO        30
#define MSG_DEBUG       45
#define MSG_DEBUG_MORE  60

extern int __msglevel; /* the higher, the more messages... */

#if defined(NDEBUG) && defined(__GNUC__)

    /* gcc's cpp has extensions; it allows for macros with a variable number of
        arguments. We use this extension here to preprocess pmesg away. */
    #define pmesg(level, format, args...)   ((void)0)
    #define pmesg_level(level)              ((void)0)
    #define DEFINE_pmesg_level(level)       ((void)0)
#else

    void pmesg(int level, char *format, ...);
    /* print a message, if it is considered significant enough.
      Adapted from [K&R2], p. 174 */
    #define pmesg_level(level)  \
        __msglevel = level;

    #define DEFINE_pmesg_level(level)  \
        int __msglevel = level;
#endif

#endif /* DEBUG_H */

