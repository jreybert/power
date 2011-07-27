
CC=gcc
CFLAGS=-O3 -g
LDFLAGS=-L $(PAPI_ROOT)/lib/ -lpapi -lpthread -lncurses

all: trace_power_and_cycles trace_power_and_cycles_multiplex trace_power_and_cycles_file

trace_power_and_cycles: trace_power_and_cycles.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
trace_power_and_cycles_file: trace_power_and_cycles_file.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

trace_power_and_cycles_multiplex: trace_power_and_cycles_multiplex.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)


.PHONY: clean

clean:
	rm -f *.o trace_power_and_cycles *~
