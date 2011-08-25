
CC=gcc 
CFLAGS=-g -I. `pkg-config --cflags --libs glib-2.0` -DCOMPILE_WITH_SENSORS
LDFLAGS=-L $(PAPI_ROOT)/lib/ -lpapi -lpthread -lsensors

#SRC=power.c cstates.c freq.c
#OBJS=
#
#all: power
#
#power: $(OBJS)
#	$(GCC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

all: power
objects := $(patsubst %.c,%.o,$(wildcard *.c))

power: $(objects)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(objects)


.PHONY: clean

clean:
	rm -f *.o power *~
