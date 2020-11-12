set datafile separator ','
set xdata time # tells gnuplot the x axis is time data
set timefmt "%m/%d/%y %H:%M:%S" # specify our time string format
set format x "%H:%M:%S" # otherwise it will show only MM:SS
set xtics rotate # rotate labels on the x axis
#set xrange ["03/30/20 14:47:08":"03/30/20 14:59:58"]

set y2tics # enable second axis
set ytics nomirror # dont show the tics on that side
set y2label "RPM" # label for second axis
set y2range [0:]

plot 'log.csv' using 9:7 title "Oil Pressure" with lines,'' using 9:1 title "RPM" axis x1y2 with lines, '' using 9:6 title "Oil Temperature", '' using 9:5 title "Battery Voltage", '' using 9:4 title "Switched Voltage", '' using 9:10 title "Lambda" with lines, '' using 9:11 title "IAP(kpa)

pause mouse close
