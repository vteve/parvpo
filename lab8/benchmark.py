import subprocess
import time
import os
import csv
import matplotlib.pyplot as plt

cpp_file = 'main.cpp'
exe_template = 'main_{opt}_{lto}.exe'
csv_file = 'results_lto.csv'

# Оптимизации компилятора и компоновщика
opt_levels = ['O2', 'Oz']
lto_levels = [
    ('noLTO', []),
    ('thinLTO', ['-flto=thin']),
    ('fullLTO', ['-flto'])
]

results = []

for opt in opt_levels:
    for lto_name, lto_flags in lto_levels:
        exe_file = exe_template.format(opt=opt, lto=lto_name)
        build_cmd = ['g++', f'-{opt}', '-o', exe_file, cpp_file] + lto_flags

        # Время сборки
        start_build = time.perf_counter()
        build = subprocess.run(build_cmd, capture_output=True)
        end_build = time.perf_counter()
        build_time = end_build - start_build

        if build.returncode != 0:
            print(f'Ошибка сборки для -{opt} {lto_name}:', build.stderr.decode())
            continue

        # Размер файла
        file_size = os.path.getsize(exe_file) / 1024  # КБ

        # Время выполнения
        start_run = time.perf_counter()
        run = subprocess.run([exe_file], capture_output=True)
        end_run = time.perf_counter()
        run_time = end_run - start_run

        results.append({
            'opt': opt,
            'lto': lto_name,
            'build_time': build_time,
            'run_time': run_time,
            'file_size': file_size
        })

# Сохраняем в CSV
with open(csv_file, 'w', newline='', encoding='utf-8') as f:
    writer = csv.DictWriter(f, fieldnames=['opt', 'lto', 'build_time', 'run_time', 'file_size'])
    writer.writeheader()
    for row in results:
        writer.writerow(row)

print(f'Результаты сохранены в {csv_file}')

# Строим графики
labels = [f"{r['opt']}\n{r['lto']}" for r in results]
build_times = [r['build_time'] for r in results]
run_times = [r['run_time'] for r in results]
file_sizes = [r['file_size'] for r in results]

plt.figure(figsize=(15, 4))

plt.subplot(1, 3, 1)
plt.bar(labels, build_times, color='skyblue')
plt.title('Время сборки (сек)')
plt.xlabel('Оптимизация')
plt.ylabel('Время, сек')

plt.subplot(1, 3, 2)
plt.bar(labels, run_times, color='lightgreen')
plt.title('Время выполнения (сек)')
plt.xlabel('Оптимизация')
plt.ylabel('Время, сек')

plt.subplot(1, 3, 3)
plt.bar(labels, file_sizes, color='salmon')
plt.title('Размер файла (КБ)')
plt.xlabel('Оптимизация')
plt.ylabel('Размер, КБ')

plt.tight_layout()
plt.savefig('benchmark_lto_results.png')
plt.show()