set terminal png font "Arial_Black" 11 transparent nointerlace truecolor  size 1024, 768 nocrop enhanced
set output './commandExecution.png'
unset border
set size 1.0, 1.0
set origin 0.0, 0.0
unset key
set xrange [0:*] noreverse nowriteback
set yrange [0 : *] noreverse nowriteback
unset ytics
set mxtics
set style fill  solid 1.00 border -1
set ylabel "Execution Time (cycles)" offset character .05, 0,0 font "" textcolor lt -1 rotate by 90
set xlabel "Number of Commands with this Execution Time"
set boxwidth 1.00 relative
set title "Command Execution Time"  offset character 0, -1, 0 font "" norotate 

plot '-' using 1:2:(1) with boxes
@data@
e
unset output
#exit
