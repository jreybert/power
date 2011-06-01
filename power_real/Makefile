
CC=gcc
CFLAGS=-O3
LDFLAGS=-L /usr/local/papi/4.1.3/lib/ -lpapi -lpthread -lncurses

all: trace_power_and_cycles

trace_power_and_cycles: trace_power_and_cycles.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)


.PHONY: clean

clean:
	rm -f *.o trace_power_and_cycles *~
