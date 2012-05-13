#include "io.h"
#include "interrupt.h"

extern void timer0_periodic_task();

void __attribute__ ((interrupt("FIQ"))) fiq_isr(void) {
    T0IR = BIT0; /* Clear interrupt */
    timer0_periodic_task();
}

void timer0Init() {
    T0MR0 = 2999;	  /* Count 1 millisecond (1/1000 sec) */
    T0MCR = BIT0 | BIT1;  /* reset & Interrupt on match */
    T0TCR = BIT0;         /* start timer */

    VICIntSelect = BIT4;
    VICIntEnable = BIT4;

    enable_interrupts();
}
