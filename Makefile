
.DELETE_ON_ERROR:

# Prove we started from main Makefile
export STARTEDATTOP=true

LPC2148_OPTS=-DUART0_DEBUG -DCLOCK_CONF_SECOND=10 -DTRACE

SUBDIRS =
# arch
SUBDIRS += arch/lpc21xx/uart 
SUBDIRS += arch/lpc21xx/enc28j60 
SUBDIRS += arch/lpc21xx/spi_eth 
SUBDIRS += arch/lpc21xx/spi1
SUBDIRS += arch/lpc21xx/timer 
SUBDIRS += arch/lpc21xx/clock 
SUBDIRS += arch/lpc21xx/lcd
SUBDIRS += arch/lpc21xx/rtc
#SUBDIRS += arch/lpc21xx/vic

SUBDIRS += lib 
SUBDIRS += uip 

SUBDIRS += apps/alarm-watch-server
SUBDIRS += apps/dhcpc
SUBDIRS += apps/alarm-watch


TARGET = main

SRC = $(TARGET).c syscalls.c debug.c 

ASRC=startup.S
LDSCRIPT=lpc2148-flash

# Commands
TOOLCHAIN_DIR=/home/tomer/gnuarm-4.0.2/bin/
export CC=$(TOOLCHAIN_DIR)arm-elf-gcc
export AR=$(TOOLCHAIN_DIR)arm-elf-ar
export OBJCOPY=$(TOOLCHAIN_DIR)arm-elf-objcopy
export OBJDUMP=$(TOOLCHAIN_DIR)arm-elf-objdump
export ELFSIZE=$(TOOLCHAIN_DIR)arm-elf-size

export ROOT=$(shell pwd)
export COMMON=$(ROOT)/common

#INCLUDES = -I/usr/include
INCLUDES = -I/home/tomer/gnuarm-4.0.2/

INCLUDES += -I$(ROOT)/lib 
INCLUDES += -I$(ROOT)/uip 

INCLUDES += -I$(ROOT)/arch/lpc21xx/spi1 
INCLUDES += -I$(ROOT)/arch/lpc21xx/spi_eth 

INCLUDES += -I$(ROOT)/arch/lpc21xx/uart 
INCLUDES += -I$(ROOT)/arch/lpc21xx/enc28j60 
INCLUDES += -I$(ROOT)/arch/lpc21xx/lcd  
INCLUDES += -I$(ROOT)/arch/lpc21xx/clock 
INCLUDES += -I$(ROOT)/arch/lpc21xx/timer
INCLUDES += -I$(ROOT)/arch/lpc21xx/rtc
#INCLUDES += -I$(ROOT)/arch/lpc21xx/vic

# apps
INCLUDES += -I$(ROOT)/apps/dhcpc
INCLUDES += -I$(ROOT)/apps/alarm-watch-server
INCLUDES += -I$(ROOT)/apps/alarm-watch

export BASEINCLUDE=-I$(ROOT) -I$(ROOT)/include

# Define all object files.
COBJ      = $(SRC:.c=.o) 
AOBJ      = $(ASRC:.S=.o)

#
# Flags
#
WARNINGS = -Wall 
# Treat warnings as errors
#WARNINGS += -Werror
#WARNINGS += -Wextra 
#WARNINGS += -Wshadow 
#WARNINGS += -Wpointer-arith 
#WARNINGS += -Wbad-function-cast 
#WARNINGS += -Wcast-align 
#WARNINGS += -Wsign-compare 
#WARNINGS += -Waggregate-return 
#WARNINGS += -Wstrict-prototypes 
#WARNINGS += -Wmissing-prototypes 
#WARNINGS += -Wmissing-declarations 
#WARNINGS += -Wunused
export WARNINGS

CFLAGS += $(WARNINGS)
CFLAGS += $(INCLUDES) 
CFLAGS += $(BASEINCLUDE)
CFLAGS += -mcpu=arm7tdmi 

# Produce debugging info in the OS's native format
#CFLAGS += -gdwarf-2

# Optimize for size
CFLAGS += -Os

CFLAGS += -std=gnu99
CFLAGS += $(LPC2148_OPTS)
export CFLAGS

LDFLAGS = $(COMMON)/common.a -lc -lm -lgcc -nostartfiles -T$(LDSCRIPT).ld --relocatable
export LDFLAGS

# Assembly flags (to as via gcc)
ASFLAGS = -I. -x assembler-with-cpp
ASFLAGS += -mcpu=arm7tdmi
ASFLAGS += $(ADEFS) 
ASFLAGS += -Wall -gdwarf-2

# Define all listing files.
#LST = $(ASRC:.S=.lst) $(SRC:.c=.lst) 

.PHONY: all build bin elf hex
all: build

build: bin
bin: subdirs hex
	@echo "Done bin"

elf: $(TARGET).elf
hex: $(TARGET).hex

.PHONY: subdirs $(SUBDIRS)
subdirs: $(SUBDIRS)
	@echo "Done subdirs"

# Make all SUBDIRS
$(SUBDIRS):
	$(MAKE) -C $@

#
# Program the device
#

# verbose output:
#LPC21ISP_DEBUG = -debug
LPC21ISP = /home/tomer/LPC21ISP/lpc21isp_147
LPC21ISP_BAUD = 19200
LPC21ISP_XTAL = 12000
LPC21ISP_FLASHFILE = $(TARGET).hex
LPC21ISP_PORT = /dev/ttyUSB0
#SHOW_TERMINAL=-term
LPC21ISP_VERIFY=-verify
# enter bootloader via RS232 DTR/RTS (only if hardware supports this feature - see Philips AppNote):
LPC21ISP_CONTROL = -control

.PHONY: program
program: bin
	$(LPC21ISP) $(SHOW_TERMINAL) $(LPC21ISP_VERIFY) $(LPC21ISP_CONTROL) $(LPC21ISP_DEBUG) $(LPC21ISP_FLASHFILE) $(LPC21ISP_PORT) $(LPC21ISP_BAUD) $(LPC21ISP_XTAL)
	@echo "Download done."

.PHONY: terminal
terminal:
	$(LPC21ISP) -termonly $(LPC21ISP_DEBUG) $(LPC21ISP_FLASHFILE) $(LPC21ISP_PORT) $(LPC21ISP_BAUD) $(LPC21ISP_XTAL)

# Create final output file (.hex) from ELF output file.
%.hex: %.elf
	$(OBJCOPY) --strip-debug -O ihex $< $@

# Link: create ELF output file from object files.
.SECONDARY : $(TARGET).elf
.PRECIOUS : $(AOBJ) $(COBJ)
%.elf: $(AOBJ) $(COBJ) $(LDSCRIPT).ld $(COMMON)/common.a
	$(CC) $(CFLAGS) $(AOBJ) $(COBJ) --output $@ $(LDFLAGS)
	$(ELFSIZE) -A $(TARGET).elf

# Compile: create object files from C source files.
$(COBJ) : %.o : %.c Makefile .depend
	$(CC) -c $(CFLAGS) $< -o $@ 

# Assemble: create object files from assembler source files.
$(AOBJ) : %.o : %.S
	$(CC) -c $(ASFLAGS) $< -o $@

#
#  The .depend files contains the list of header files that the
#  various source files depend on.  By doing this, we'll only
#  rebuild the .o's that are affected by header files changing.
#
.depend:
	$(CC) $(CFLAGS) -M $(SRC) -o .depend

#
#  Utility targets
#
.PHONY: tags
tags :
	@rm -f tags ctags
	find . -name \*.c -exec ctags -a {} \;
	find . -name \*.h -exec ctags -a {} \;

.PHONY: clean
clean :
	find . -name \*.o -exec rm -f {} \;
	find . -name .depend -exec rm -f {} \;
	rm -f *.map *.lst *.elf *.hex .depend sizes.csv tags
	rm $(COMMON)/common.a

ifeq (.depend,$(wildcard .depend))
include .depend
endif
