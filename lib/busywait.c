#include "io.h"
#include "interrupt.h"

volatile double g_calib = 0;

void busywaitInit()
{
    volatile int i = 0;

    return;

    T0TC = 0;
    T0TCR = BIT0;      /* start timer */
    for (i = 0; i < 100000; i++)
    {
	g_calib = 0;
    }
    T0TCR = 0; /* stop timer */
    g_calib = T0TC; 
    g_calib = 100000.0/g_calib; /* ratio of instrucitons per timer clock ticks */

    g_calib = g_calib * 4.0; /* ratio of instructions per cpu clock ticks */

    g_calib = g_calib / 1.2;	/* ratio of instructions per microsec */
}


void busywait(uint32_t microseconds)
{
    volatile double f;
    volatile int i = 0;
    //uint32_t limit = (double)microseconds * g_calib;
    uint32_t limit =  microseconds / 5;
    for(i = 0; i < limit; i++)
    {
	f = 2;
    }
}


