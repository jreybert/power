
GCC=gcc 
CFLAGS= -O0
LDFLAGS=-L/usr/local/papi/4.1.1/lib -lpapi

all: access-papi access-papi-native

access-papi: access-papi.c 
	$(GCC) $(CFLAGS) -o $@ $< $(LDFLAGS)

access-papi-native: access-papi-native.c 
	$(GCC) $(CFLAGS) -o $@ $< $(LDFLAGS)

.PHONY: clean

clean:
	rm -f access-papi access-papi-native *~
