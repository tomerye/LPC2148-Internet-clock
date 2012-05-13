SRC_FILES = httpd.c http-strings.c httpd-fs.c httpd-cgi.c
INCLUDES = -I../../uip -I../../fat
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
	$(CC) -c $(CFLAGS) $(INCLUDES) -Wno-cast-align $< -o $@
	$(AR) rc $(COMMON)/common.a $@

#
#  The .depend files contains the list of header files that the
#  various source files depend on.  By doing this, we'll only
#  rebuild the .o's that are affected by header files changing.
#
.depend:
	$(CC) $(CFLAGS) $(INCLUDES) -M $(SRC_FILES) -o .depend

ifeq (.depend,$(wildcard .depend))
include .depend
endif
