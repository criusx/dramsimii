set terminal png font arial 11 transparent nointerlace truecolor  size 1024, 768 nocrop enhanced
set output './power2.png'
unset border
#set bmargin 0.01
#set lmargin 0.01
#set rmargin 0.05
#set tmargin 0.05
reset

set size 1.0, 1.0
set origin 0.0, 0.0

set multiplot 

set size 1.0, 0.52
set origin 0,0

set autoscale x
set style fill  solid 1.00 border -1
set grid nopolar
set grid noxtics nomxtics ytics nomytics noztics nomztics nox2tics nomx2tics noy2tics nomy2tics nocbtics nomcbtics

#set key outside above left vertical Right reverse noinvert enhanced samplen 4 spacing 0.1 width 0 height 0 autotitles columnhead box linetype -2 linewidth 0.5
set key outside center rm Right reverse noinvert enhanced samplen 4   autotitles columnhead box linetype -2 linewidth 0.5
#unset key

set style histogram rowstacked title  offset 0,0,0
set style data histograms

set xtics border in scale 1,1 nomirror rotate by -45  offset character 0,0,0 0,10 
set yrange [ 0.00000 : 400.000 ] noreverse nowriteback

set xrange [0:304] noreverse nowriteback
#set x2range [0:<<x2range>>] noreverse nowriteback
set ylabel "Power (mW)" offset character .05, 0, 0 font "" textcolor lt -1 rotate by 90
set title "Power Consumed vs. Time"  offset character 0, -1, 0 font "" norotate 
#plot 'powerTest.dat' using 1 t "Psys(ACT_STBY)[0][0]", '' using 2 t "Psys(ACT)[0][0]", '' using 3 t "P(ACT_STBY)[0][1]", '' using 4 t "Psys(ACT)[0][1]", '' using 5 t "Psys(ACT)", '' using 6 t "Psys(RD)", '' using 7 t "Psys(WR)"
plot './powerTest.dat' \
u 2 t "a", \
'' u 3 t "b", \
'' u 4 t "c", \
'' u 5 t "d", \
'' u 6 t "e", \
'' u 7 t "f", \
'' u 8 t "g", \
'' u 9 t "h", \
'' u 10 t "i", \
'' u 11 t "j", \
'' u 12 t "k"

#unset style
#set yrange [1 : 4209] noreverse nowriteback
#set logscale y
#unset key

#set size 1.0, 0.52
#set origin 0, 0.5

#set xrange [0:253] noreverse nowriteback
#unset style
#set style fill  solid 1.00 border -1
#set x2range [0:<<x2range>>] noreverse nowriteback
#set ylabel "Number of Transactions" offset character .05, 0, 0 font "" textcolor lt -1 rotate by 90
#set title "Number of Transactions vs. Transaction Time"  offset character 0, -1, 0 font "" norotate 
#plot 'standardOutFile004-power.txt' using 2
#plot 'standardOutFile004-stats.txt' using 1:2 with boxes

#set title "Power" 0,.995

unset multiplot
unset output