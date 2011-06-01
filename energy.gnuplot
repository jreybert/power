#!/usr/bin/gnuplot -p


set xlabel "time (s)"
set ylabel "power (W)"

set key left outside

#plot "trace_perf" u 1:2  w lp  t "core 1" , \
#     "trace_perf" u 1:6  w lp  t "core 2" , \
#     "trace_perf" u 1:10  w lp  t "core 3" , \
#     "trace_perf" u 1:14  w lp t "core 4" , \
#     "trace_perf" u 1:18  w lp t "core 5" , \
#     "trace_perf" u 1:22  w lp t "core 6" , \
#     "trace_perf" u 1:26  w lp t "core 7" , \
#     "trace_perf" u 1:30  w lp t "core 8" 
plot "trace_perf" u 1:3  w lp t "core 1" , \
     "trace_perf" u 1:4  w lp t "core 2" , \
     "trace_perf" u 1:5  w lp t "core 3" , \
     "trace_perf" u 1:6  w lp t "core 4" , \
     "trace_perf" u 1:7  w lp t "core 5" , \
     "trace_perf" u 1:8  w lp t "core 6" , \
     "trace_perf" u 1:9  w lp t "core 7" , \
     "trace_perf" u 1:10 w lp t "core 8" , \
     "trace_perf" u 1:11 w lp t "core 9" , \
     "trace_perf" u 1:12 w lp t "core 10" , \
     "trace_perf" u 1:13 w lp t "core 11" , \
     "trace_perf" u 1:14 w lp t "core 12" , \
     "trace_perf" u 1:15 w lp t "core 13" , \
     "trace_perf" u 1:16 w lp t "core 14" , \
     "trace_perf" u 1:17 w lp t "core 15" , \
     "trace_perf" u 1:18 w lp t "core 16" , \
     "trace_perf" u 1:19 w lp t "core 17" , \
     "trace_perf" u 1:20 w lp t "core 18" , \
     "trace_perf" u 1:21 w lp t "core 19" , \
     "trace_perf" u 1:22 w lp t "core 20" , \
     "trace_perf" u 1:23 w lp t "core 21" , \
     "trace_perf" u 1:24 w lp t "core 22" , \
     "trace_perf" u 1:25 w lp t "core 23" , \
     "trace_perf" u 1:26 w lp t "core 24" , \
     "trace_perf" u 1:27 w lp t "core 25" , \
     "trace_perf" u 1:28 w lp t "core 26" , \
     "trace_perf" u 1:29 w lp t "core 27" , \
     "trace_perf" u 1:30 w lp t "core 28" , \
     "trace_perf" u 1:31 w lp t "core 29" , \
     "trace_perf" u 1:32 w lp t "core 30" , \
     "trace_perf" u 1:33 w lp t "core 31" , \
     "trace_perf" u 1:34 w lp t "core 32" 
