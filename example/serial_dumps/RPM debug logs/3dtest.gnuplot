set datafile separator ','
set hidden3d
set key off
set yrange [-20:110]
set zrange [.6:1.4]
set xrange [1000:10000]
set dgrid3d 50,50 gauss 1
#set dgrid3d 50,50 qnorm 2

splot 'system_log.csv' using 1:10:9 with lines
#splot 'system_log.csv' using 1:9:10 with points palette pointsize 3 pointtype 7
