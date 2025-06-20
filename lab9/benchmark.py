import subprocess
import time
import os
import csv
import re
import pandas as pd
import matplotlib.pyplot as plt
from tabulate import tabulate

import dataframe_image as dfi

cpp_file = 'main.cpp'
exe_template = 'main_{}.exe'
csv_file = 'results_lab9.csv'
table_img = 'results_lab9_table.png'
opt_levels = ['O0', 'O1', 'O2', 'O3', 'Os', 'Ofast', 'Og']

results = []

# Компиляция и запуск
for opt in opt_levels:
    exe_file = exe_template.format(opt)
    build_cmd = ['g++', f'-{opt}', '-fopenmp', '-o', exe_file, cpp_file]
    print(f'Собираю: {" ".join(build_cmd)}')
    start_build = time.perf_counter()
    build = subprocess.run(build_cmd, capture_output=True)
    end_build = time.perf_counter()
    build_time = end_build - start_build

    if build.returncode != 0:
        print(f'Ошибка сборки для -{opt}:', build.stderr.decode())
        continue

    print(f'Запускаю: {exe_file}')
    start_run = time.perf_counter()
    run = subprocess.run([exe_file], capture_output=True, encoding='utf-8')
    end_run = time.perf_counter()
    run_time = end_run - start_run

    # Парсим вывод
    for line in run.stdout.splitlines():
        # baseline
        m = re.match(
            r'Variant: BASELINE_(OMP|NOOMP) \| bound=(\w+) \| OpenMP=(ON|OFF) \| Time= ?([\d.]+) s \| Roots=(\d+)',
            line.strip()
        )
        if m:
            results.append({
                'opt': opt,
                'mode': 'baseline',
                'bound': m.group(2),
                'OpenMP': m.group(3),
                'time': float(m.group(4))
            })
            continue
        # остальные режимы
        m = re.match(
            r'Variant: (\w+)_ALL_(OMP|NOOMP) \| bound=(\w+) \| OpenMP=(ON|OFF) \| Time= ?([\d.]+) s \| Roots=(\d+)',
            line.strip()
        )
        if m:
            results.append({
                'opt': opt,
                'mode': m.group(1).lower(),
                'bound': m.group(3),
                'OpenMP': m.group(4),
                'time': float(m.group(5))
            })

# Сохраняем в CSV
with open(csv_file, 'w', newline='', encoding='utf-8') as f:
    writer = csv.DictWriter(f, fieldnames=['opt', 'mode', 'bound', 'OpenMP', 'time'])
    writer.writeheader()
    for row in results:
        writer.writerow(row)

print(f'Результаты сохранены в {csv_file}')

# Строим DataFrame
df = pd.DataFrame(results)
if df.empty:
    print("Нет данных для построения таблицы.")
    exit(1)

# Красивое отображение mode
df['mode'] = df['mode'].replace({
    'baseline': 'baseline',
    'const': 'const_all',
    'volatile': 'volatile_all',
    'restrict': 'restrict_all'
})

# Формируем итоговую таблицу
df['Компиляция'] = df['opt']
df['Режим'] = df['mode']
df['Граница'] = df['bound']
df['OpenMP'] = df['OpenMP']
df['Время (сек)'] = df['time']

table_df = df[['Компиляция', 'Режим', 'Граница', 'OpenMP', 'Время (сек)']]

# Сохраняем таблицу как PNG (если установлен dataframe_image)
if dfi is not None:
    dfi.export(pd.DataFrame(table_df), table_img)
    print(f'Таблица сохранена как {table_img}')
else:
    # Альтернатива: сохранить как картинку через matplotlib
    fig, ax = plt.subplots(figsize=(12, 0.5 + 0.3 * len(table_df)))
    ax.axis('off')
    mpl_table = ax.table(
        cellText=table_df.values.tolist(),
        colLabels=list(table_df.columns),
        loc='center',
        cellLoc='center'
    )
    mpl_table.auto_set_font_size(False)
    mpl_table.set_fontsize(10)
    mpl_table.auto_set_column_width(col=list(range(len(table_df.columns))))
    plt.tight_layout()
    plt.savefig(table_img)
    print(f'Таблица сохранена как {table_img} (matplotlib)')

# Также выводим в консоль красиво
print(tabulate(table_df, headers='keys', tablefmt='github', floatfmt=".4f"))