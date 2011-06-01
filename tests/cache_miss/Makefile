
GCC=gcc -g -O0 -I../perfcount -L/usr/local/papi-4.1.1/lib -lpapi

all: mull-unopt mull-opt acc-opt acc-unopt

acc-unopt: access-unopt.c 
	gcc -c ../perfcount/perfcount.c -o perf.o
	$(GCC) -o $@ $< perf.o

acc-opt: access-opt.c 
	gcc -c ../perfcount/perfcount.c -o perf.o
	$(GCC) -o $@ $< perf.o
mull-unopt: mull-unopt.c 
	gcc -c ../perfcount/perfcount.c -o perf.o
	$(GCC) -o $@ $< perf.o

mull-opt: mull-opt.c 
	gcc -c ../perfcount/perfcount.c -o perf.o
	$(GCC) -o $@ $< perf.o

.PHONY: vtune-unopt-hotspot vtune-unopt-cache 
	 
vtune-unopt-hotspot: mull-unopt 
	mkdir -p $@ 
	amplxe-cl --collect hotspots -follow-child -target-duration-type=long -allow-multiple-runs -no-analyze-system -data-limit=100 -slow-frames-threshold=40 -fast-frames-threshold=100 -- /home/jreybert/mentors/src/cache2/mull-unopt

vtune-unopt-cache: mull-unopt
	amplxe-cl --collect nehalem_memory-access -follow-child -target-duration-type=short -no-allow-multiple-runs -no-analyze-system -data-limit=100 -slow-frames-threshold=40 -fast-frames-threshold=100 -- /home/jreybert/mentors/src/cache2/mull-unopt

vtune-opt-hotspot: mull-opt 
	mkdir -p $@ 
	amplxe-cl --collect hotspots -follow-child -target-duration-type=long -allow-multiple-runs -no-analyze-system -data-limit=100 -slow-frames-threshold=40 -fast-frames-threshold=100 -- /home/jreybert/mentors/src/cache2/mull-opt

vtune-opt-cache: mull-opt
	amplxe-cl --collect nehalem_memory-access -follow-child -target-duration-type=short -no-allow-multiple-runs -no-analyze-system -data-limit=100 -slow-frames-threshold=40 -fast-frames-threshold=100 -- /home/jreybert/mentors/src/cache2/mull-opt
