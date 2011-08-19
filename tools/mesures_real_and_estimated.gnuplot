#!/usr/bin/gnuplot -p

#set yrange [0 to 20]

set xlabel "time (s)"
set ylabel "power (W)"


f="data_global.gnuplot"

set key left outside

plot f u 1:3  w lp  t "Real" , \
     f u 1:2  w lp  t "Estimated", \
     f u 1:($4 / 2)  w lp  t "Temp" 
