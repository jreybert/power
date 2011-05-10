#!/usr/bin/gnuplot -p


plot "trace_perf" u 1:2 w lp t "cpu 1", \
     "trace_perf" u 1:3 w lp t "cpu 2"
