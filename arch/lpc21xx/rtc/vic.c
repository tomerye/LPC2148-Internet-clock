
#include <interrupt.h>
#include "type.h"
#include "vic.h"
#include "lpc214x.h"
#include "io.h"


static void __attribute__ ((interrupt("IRQ")))
_vicDefaultIsr(void) {
    vicUpdatePriority();
}

static void vicInit(void) {
    VICDefVectAddr = (uint32_t) _vicDefaultIsr;
}

static volatile uint32_t* address_register = &VICVectAddr0;
static volatile uint32_t* control_register = &VICVectCntl0;

static void vicEnableFIQ(vic_source_t c) {
    VICIntSelect |= (1 << c); /* generate FIQ interrupt */
    VICIntEnable |= (1 << c);
}

static void vicEnableIRQ(vic_source_t c,
        uint32_t            priority,
        interrupt_handler_t h) {
    address_register[priority] = (uint32_t) h;
    control_register[priority] = c | (1<<5);
    VICIntSelect &= ~(1 << c); /* generate IRQ interrupt */
    VICIntEnable = (1 << c);
}

static void vicEnableDefaultIRQ(interrupt_handler_t h) {
    VICDefVectAddr = (uint32_t) h;
}

static inline void vicEnable(vic_source_t c) {
    VICIntEnable = (1 << c);
}

static inline void vicDisable(vic_source_t c) {
    VICIntEnClr = (1 << c);
}

static inline interrupt_source_state_t vicSaveAndDisable(vic_source_t c) {
    interrupts_state_t is = interruptsSaveAndDisable();
    interrupt_source_state_t vs = VICIntEnable;
    VICIntEnClr = 0; // to make sure the next write has an effect; see manual
    VICIntEnClr = (1 << c);
    interruptsRestore(is);
    return vs;
}

static inline void vicRestore(interrupt_source_state_t s) {
    VICIntEnable = s;
}

static inline void vicSoftSignal(vic_source_t c) {
    VICSoftInt = (1 << c);
}

static inline void vicSoftClear(vic_source_t c) {
    VICSoftIntClr = (1 << c);
}

static void ramvectorsInit(void) {
    uint32_t i;
    uint32_t* src = (uint32_t*) 0x40000200;
    uint32_t* dst = (uint32_t*) 0x40000000;

    for (i=0; i<16; i++) {
        *dst = *src;
        src  ++;
        dst++;
    }

    MEMMAP = BIT1;
}
