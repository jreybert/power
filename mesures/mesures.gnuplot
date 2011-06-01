#!/usr/bin/gnuplot -p

#set yrange [0 to 20]

#set xlabel "time (s)"
set ylabel "power per core (W)"

#set key left outside
set style data histogram

plot 'mesures.data' using 1:xtic(2) ti col
