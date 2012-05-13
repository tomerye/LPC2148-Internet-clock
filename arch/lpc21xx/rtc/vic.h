
#ifndef VIC_H_
#define VIC_H_

#include "type.h"

#define vicUpdatePriority() (VICVectAddr=0xff)

typedef uint32_t interrupt_source_state_t;

typedef void (*interrupt_handler_t)(void);

typedef enum {
    INT_SOURCE_WDT    = 0,
    INT_SOURCE_SWI    = 1,
    INT_SOURCE_CORE1  = 2,
    INT_SOURCE_CORE2  = 3,
    INT_SOURCE_TIMER0 = 4,
    INT_SOURCE_TIMER1 = 5,
    INT_SOURCE_UART0  = 6,
    INT_SOURCE_UART1  = 7,
    INT_SOURCE_PWM0   = 8,
    INT_SOURCE_I2C0   = 9,
    INT_SOURCE_SPI0   = 10,
    INT_SOURCE_SPI1   = 11,
    INT_SOURCE_PLL    = 12,
    INT_SOURCE_RTC    = 13,
    INT_SOURCE_EINT0  = 14,
    INT_SOURCE_EINT1  = 15,
    INT_SOURCE_EINT2  = 16,
    INT_SOURCE_EINT3  = 17,
    INT_SOURCE_ADC    = 18,
    INT_SOURCE_I2C1   = 19,
    INT_SOURCE_BOD    = 20,
    INT_SOURCE_ADC1   = 21,
    INT_SOURCE_USB    = 22
} vic_source_t;


#define disable_interrupts()											\
  asm volatile (															\
		"STMDB	SP!, {R0}		\n\t"	/* Push R0.						*/	\
		"MRS	R0, CPSR		\n\t"	/* Get CPSR.					*/	\
		"ORR	R0, R0, #0xC0	\n\t"	/* Disable IRQ, FIQ.			*/	\
		"MSR	CPSR, R0		\n\t"	/* Write back modified value.	*/	\
		"LDMIA	SP!, {R0}			" )	/* Pop R0.						*/

#define enable_interrupts()												\
  asm volatile (															\
		"STMDB	SP!, {R0}		\n\t"	/* Push R0.						*/	\
		"MRS	R0, CPSR		\n\t"	/* Get CPSR.					*/	\
		"BIC	R0, R0, #0xC0	\n\t"	/* Enable IRQ, FIQ.				*/	\
		"MSR	CPSR, R0		\n\t"	/* Write back modified value.	*/	\
		"LDMIA	SP!, {R0}			" )	/* Pop R0.						*/

typedef uint32_t interrupts_state_t;

#ifndef __thumb__

static inline interrupts_state_t interruptsSaveAndEnable(void) {
  uint32_t cpsr;
  asm volatile (
        "mrs %0,cpsr;"
        "mrs r4,cpsr;"
        "bic r4,r4,#0xC0;"
        "msr cpsr,r4"
        : "=r" (cpsr)
        : /* no inputs */
        : "r4" );
  return cpsr;
}

static inline interrupts_state_t interruptsSaveAndDisable(void) {
  uint32_t cpsr;
  asm volatile (
        "mrs %0,cpsr;"
        "mrs r4,cpsr;"
        "orr r4,r4,#0xC0;"
        "msr cpsr,r4"
        : "=r" (cpsr)
        : /* no inputs */
        : "r4" );
  return cpsr;
}

// disable IRQ only, leave FIQ enabled
static inline interrupts_state_t interruptsSaveAndDisableLow(void) {
  uint32_t cpsr;
  asm volatile (
        "mrs %0,cpsr;"
        "mrs r4,cpsr;"
        "orr r4,r4,#0x80;"
        "msr cpsr,r4"
        : "=r" (cpsr)
        : /* no inputs */
        : "r4" );
  return cpsr;
}

static inline void interruptsRestore(interrupts_state_t cpsr) {
  asm volatile (
      "mrs r3,cpsr;"
      "and r4,%0,#0xC0;"
      "bic r3,r3,#0xC0;"
      "orr r3,r3,r4;"
      "msr cpsr,r3"
      :
      : "r" (cpsr)
      : "r3", "r4"
      );
}

static inline void interruptsEnable(void) {
    asm volatile (
        "mrs r3,cpsr;"
        "bic r3,r3,#0xC0;"
        "msr cpsr,r3"
        :
        :
        : "r3"
        );
}

#else /* __thumb__ */
#error "interrupt state manipulation not supported in thumb mode"
#endif
/*
static void __attribute__ ((interrupt("IRQ")))
_vicDefaultIsr(void) ;
static void vicInit(void);
static void vicEnableFIQ(vic_source_t c);
static void vicEnableIRQ(vic_source_t c,uint32_t priority,interrupt_handler_t h) ;
static void vicEnableDefaultIRQ(interrupt_handler_t h);
static inline void vicEnable(vic_source_t c);
static inline void vicDisable(vic_source_t c);
static inline interrupt_source_state_t vicSaveAndDisable(vic_source_t c);
static inline void vicRestore(interrupt_source_state_t s);
static inline void vicSoftSignal(vic_source_t c);
static inline void vicSoftClear(vic_source_t c);
static void ramvectorsInit(void);
*/

#endif /* VIC_H_ */
