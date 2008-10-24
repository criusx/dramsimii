set terminal png font "Arial_Black" 11 transparent nointerlace truecolor  size 1024, 768 nocrop enhanced
set output './delay.png'
unset border
#set bmargin 0.01
#set lmargin 0.01
#set rmargin 0.05
#set tmargin 0.05
#reset

set size 1.0, 1.0
set origin 0.0, 0.0


#unset style
#
#set logscale y
unset key

#set size 1.0, 1.0
#set origin 0, 0.5

set xrange [0:*] noreverse nowriteback
set yrange [0 : *] noreverse nowriteback
unset ytics
set mxtics
#unset style
set style fill  solid 1.00 border -1
#set x2range [0:<<x2range>>] noreverse nowriteback
set ylabel "Execution Time (cycles)" offset character .05, 0,0 font "" textcolor lt -1 rotate by 90
set xlabel "Number of Transactions with this Execution Time"
set title "Transaction Execution Time"  offset character 0, -1, 0 font "" norotate 
#plot 'standardOutFile004-power.txt' using 2
plot 'delay.dat' using 1:2 with boxes

set title "Power" 0,.995

unset multiplot
unset output
