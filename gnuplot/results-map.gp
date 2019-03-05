reset
set terminal pdf color dashed enhanced font "Times New Roman,14"

#set format x "%.0f"
#set format y "%.0f"

#set xtics 1
#set ytics 1

set xrange[0:]
set yrange[0:]

set grid front

set output "images/snr-map.pdf"
	plot "../output.dat" using 1:2:3 with image
unset output
