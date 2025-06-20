set terminal pngcairo size 1280,720 enhanced font 'Arial,12'
set output 'timers_plot_2.png'
set title "Время системных вызовов A1/A2 для разных таймеров"
set xlabel "Тип таймера"
set ylabel "Время (нс)"
set style data histogram
set style histogram clustered
set style fill solid border -1
set boxwidth 0.9
set xtics rotate by -30
set grid y
set key top right

plot 'res.txt' using 2:xtic(1) title "Время инициализации", \
     '' using 3 title "Время возврата ответа"