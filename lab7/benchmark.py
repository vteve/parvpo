import subprocess
import time
import os
import csv
import matplotlib.pyplot as plt

levels = ['O0', 'O1', 'O2', 'O3', 'Os']
results = []    

cpp_file = 'main.cpp'
exe_template = 'main_{}.exe'
csv_file = 'results.csv'

for level in levels:
    exe_file = exe_template.format(level)
    # Время сборки
    start_build = time.perf_counter()
    build = subprocess.run(['g++', f'-{level}', '-o', exe_file, cpp_file], capture_output=True)
    end_build = time.perf_counter()
    build_time = end_build - start_build

    if build.returncode != 0:
        print(f'Ошибка сборки для -{level}:', build.stderr.decode())
        continue

    # Размер файла
    file_size = os.path.getsize(exe_file) / 1024  # КБ

    # Время выполнения
    start_run = time.perf_counter()
    run = subprocess.run([exe_file], capture_output=True)
    end_run = time.perf_counter()
    run_time = end_run - start_run

    results.append({
        'level': level,
        'build_time': build_time,
        'run_time': run_time,
        'file_size': file_size
    })

# Сохраняем в CSV
with open(csv_file, 'w', newline='', encoding='utf-8') as f:
    writer = csv.DictWriter(f, fieldnames=['level', 'build_time', 'run_time', 'file_size'])
    writer.writeheader()
    for row in results:
        writer.writerow(row)

print(f'Результаты сохранены в {csv_file}')

# Строим графики
levels = [r['level'] for r in results]
build_times = [r['build_time'] for r in results]
run_times = [r['run_time'] for r in results]
file_sizes = [r['file_size'] for r in results]

plt.figure(figsize=(12, 4))

plt.subplot(1, 3, 1)
plt.bar(levels, build_times, color='skyblue')
plt.title('Время сборки (сек)')
plt.xlabel('Уровень оптимизации')
plt.ylabel('Время, сек')

plt.subplot(1, 3, 2)
plt.bar(levels, run_times, color='lightgreen')
plt.title('Время выполнения (сек)')
plt.xlabel('Уровень оптимизации')
plt.ylabel('Время, сек')

plt.subplot(1, 3, 3)
plt.bar(levels, file_sizes, color='salmon')
plt.title('Размер файла (КБ)')
plt.xlabel('Уровень оптимизации')
plt.ylabel('Размер, КБ')

plt.tight_layout()
plt.savefig('benchmark_results.png')
plt.show()