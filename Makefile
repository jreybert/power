
CC=gcc 
CFLAGS=-g -I.
LDFLAGS=

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
	$(CC) -o $@ $(objects)


.PHONY: clean

clean:
	rm -f *.o power *~