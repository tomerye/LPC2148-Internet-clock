#ifndef __TRACE_H__
#define __TRACE_H__

#ifdef TRACE
    #include <stdio.h>
    #define TRACE(format , args...)     \
        do {	                        \
            printf(format, ##args);     \
        } while(0)

    #define ASSERT(expr)                                                        \
        do {                                                                    \
            if (expr) { ; }                                                     \
            else  {                                                             \
                while (1){	                                                \
                    printf("\r\nAssert failed: " #expr " (file %s line %d)",    \
                            __FILE__, (int) __LINE__ );                         \
                }                                                               \
            }                                                                   \
        } while(0)

    #define EP()  TRACE("%s Line %d", __FILE__, (int) __LINE__);

#else

    #define ASSERT(X) 	        do{}while(0)
    #define TRACE(X, reg...)    do{}while(0)
    #define EP()  	        do{}while(0)
#endif

#endif /* __TRACE_H_ */
