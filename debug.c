#include "debug.h"
#include <stdio.h>

#if defined(NDEBUG) && defined(__GNUC__)
/* Nothing. pmesg has been "defined away" in debug.h already. */

#else

void pmesg(int level, char* format, ...) {
#ifdef NDEBUG
    /* Empty body, so a good compiler will optimise calls
       to pmesg away */
#else
    va_list args;

    if (level > __msglevel) {
        return;
    }

    va_start(args, format);
    vprintf(format, args);
    va_end(args);

#endif /* NDEBUG */
#endif /* NDEBUG && __GNUC__ */
}

