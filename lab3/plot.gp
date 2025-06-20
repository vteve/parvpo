set terminal pngcairo enhanced font "Arial,12" size 1000,600
set output "time_vs_k.png"

set title "Зависимость времени выполнения программы от размера диапазона смещений k" font "Arial,14"
set xlabel "k (байт)" font "Arial,12"
set ylabel "Время (секунды)" font "Arial,12"

set style data histograms
set style fill solid border -1
set boxwidth 0.8 relative
set grid ytics

set xtics rotate by -45

plot "res.txt" using 2:xtic(1) title "Время работы" lc rgb "#1f77b4"