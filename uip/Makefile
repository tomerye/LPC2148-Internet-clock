#SRC_FILES=psock.c uip.c uip-fw.c uip-neighbor.c uip_timer.c uiplib.c uip_arch.c uip_arp.c 
SRC_FILES=psock.c uip.c uip-fw.c uip-neighbor.c uiplib.c uip_arp.c timer.c 

#
# Define all object files.
#
ARM_OBJ = $(SRC_FILES:.c=.o)

.PHONY: all
ifeq ($(STARTEDATTOP),true)
all: $(ARM_OBJ)
else
all :
	@echo "Project must be rebuilt from top level"
endif

$(ARM_OBJ) : %.o : %.c Makefile .depend
	$(CC) -c $(CFLAGS) -Wno-cast-align $< -o $@
	$(AR) rc $(COMMON)/common.a $@

clean:
	rm -f *.o .depend

#
#  The .depend files contains the list of header files that the
#  various source files depend on.  By doing this, we'll only
#  rebuild the .o's that are affected by header files changing.
#
.depend:
	$(CC) $(CFLAGS) -M $(SRC_FILES) -o .depend

ifeq (.depend,$(wildcard .depend))
include .depend
endif
