set datafile separator ','
set xdata time # tells gnuplot the x axis is time data
set timefmt "%m/%d/%y %H:%M:%S" # specify our time string format
set format x "%H:%M:%S" # otherwise it will show only MM:SS
set xtics rotate # rotate labels on the x axis
#set xrange ["03/30/20 14:47:08":"03/30/20 14:59:58"]

set ytics nomirror # dont show the tics on that side

plot '20201030.csv' using 9:($2/$3) title "IG" axis x1y1 with lines
pause 30
reread
