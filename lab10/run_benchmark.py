import subprocess
import re
import json
import matplotlib.pyplot as plt
import numpy as np
import os
import sys

os.system('chcp 65001 >nul')

def compile_program(optimization_level):
    """Компилирует программу с заданным уровнем оптимизации"""
    output_name = f"main_{optimization_level}.exe"
    cmd = f"g++ -{optimization_level} -o {output_name} main.cpp"
    
    print(f"Компилируем с -{optimization_level}...")
    result = subprocess.run(cmd, shell=True, capture_output=True, text=True)
    
    if result.returncode != 0:
        print(f"Ошибка компиляции с -{optimization_level}: {result.stderr}")
        return None
    
    return output_name

def run_program(program_name):
    """Запускает программу и возвращает её вывод"""
    print(f"Запускаем {program_name}...")
    result = subprocess.run(program_name, shell=True, capture_output=True, text=True, encoding='utf-8')
    
    if result.returncode != 0:
        print(f"Ошибка запуска {program_name}: {result.stderr}")
        return None
    
    return result.stdout

def parse_results(output_text):
    """Парсит результаты из вывода программы"""
    results = {}
    
    patterns = {
        'baseline': r'1\) Baseline.*?Time = ([\d.]+) с',
        'correct': r'2\) Верная подсказка.*?Time = ([\d.]+) с',
        'wrong': r'3\) Неверная подсказка.*?Time = ([\d.]+) с',
        'invert': r'4\) Инвертированная.*?Time = ([\d.]+) с'
    }
    
    for name, pattern in patterns.items():
        match = re.search(pattern, output_text, re.DOTALL)
        if match:
            results[name] = float(match.group(1))
        else:
            print(f"Не удалось найти результаты для {name}")
    
    return results

def create_charts(all_results):
    """Создает графики сравнения результатов"""
    optimizations = ['O0', 'O1', 'O2', 'O3', 'Os', 'Oz']
    methods = ['baseline', 'correct', 'wrong', 'invert']
    
    # Подготовка данных
    data = {}
    for method in methods:
        data[method] = []
        for opt in optimizations:
            if opt in all_results and method in all_results[opt]:
                data[method].append(all_results[opt][method])
            else:
                data[method].append(0)
    
    # Создание графика
    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(15, 6))
    
    # График 1: Время выполнения
    x = np.arange(len(optimizations))
    width = 0.2
    
    ax1.bar(x - 1.5*width, data['baseline'], width, label='Baseline', alpha=0.8)
    ax1.bar(x - 0.5*width, data['correct'], width, label='Correct hint', alpha=0.8)
    ax1.bar(x + 0.5*width, data['wrong'], width, label='Wrong hint', alpha=0.8)
    ax1.bar(x + 1.5*width, data['invert'], width, label='Inverted hint', alpha=0.8)
    
    ax1.set_xlabel('Уровень оптимизации')
    ax1.set_ylabel('Время выполнения (секунды)')
    ax1.set_title('Сравнение времени выполнения разных методов')
    ax1.set_xticks(x)
    ax1.set_xticklabels(optimizations)
    ax1.legend()
    ax1.grid(True, alpha=0.3)
    
    # График 2: Относительная производительность
    relative_data = {}
    for method in methods:
        if method == 'baseline':
            relative_data[method] = [1.0] * len(optimizations)
        else:
            relative_data[method] = []
            for i, opt in enumerate(optimizations):
                if data['baseline'][i] > 0 and data[method][i] > 0:
                    relative_data[method].append(data['baseline'][i] / data[method][i])
                else:
                    relative_data[method].append(1.0)
    
    ax2.bar(x - 0.5*width, relative_data['correct'], width, label='Correct hint', alpha=0.8)
    ax2.bar(x + 0.5*width, relative_data['wrong'], width, label='Wrong hint', alpha=0.8)
    ax2.bar(x + 1.5*width, relative_data['invert'], width, label='Inverted hint', alpha=0.8)
    
    ax2.set_xlabel('Уровень оптимизации')
    ax2.set_ylabel('Относительная производительность')
    ax2.set_title('Производительность относительно baseline')
    ax2.set_xticks(x)
    ax2.set_xticklabels(optimizations)
    ax2.legend()
    ax2.grid(True, alpha=0.3)
    ax2.axhline(y=1.0, color='black', linestyle='--', alpha=0.5)
    
    plt.tight_layout()
    plt.savefig('benchmark_results.png', dpi=300, bbox_inches='tight')
    print("График сохранен в benchmark_results.png")
    plt.show()

def save_results_to_json(all_results):
    """Сохраняет результаты в JSON файл"""
    with open('benchmark_results.json', 'w', encoding='utf-8') as f:
        json.dump(all_results, f, indent=2, ensure_ascii=False)
    print("Результаты сохранены в benchmark_results.json")

def main():
    print("=" * 50)
    print("Бенчмарк оптимизации ветвлений")
    print("=" * 50)
    
    optimizations = ['O0', 'O1', 'O2', 'O3', 'Os', 'Oz']
    all_results = {}
    
    for opt in optimizations:
        print(f"\n--- Тестирование с -{opt} ---")
        
        # Компиляция
        program_name = compile_program(opt)
        if not program_name:
            continue
        
        # Запуск
        output = run_program(program_name)
        if not output:
            continue
        
        # Парсинг результатов
        results = parse_results(output)
        if results:
            all_results[opt] = results
            print(f"Результаты для -{opt}:")
            for method, time in results.items():
                print(f"  {method}: {time:.6f} с")
        else:
            print(f"Не удалось получить результаты для -{opt}")
    
    if all_results:
        print(f"\n" + "=" * 50)
        print("Создание графиков...")
        create_charts(all_results)
        save_results_to_json(all_results)
        
        print(f"\n" + "=" * 50)
        print("Итоговая сводка:")
        for opt in optimizations:
            if opt in all_results:
                baseline = all_results[opt].get('baseline', 0)
                correct = all_results[opt].get('correct', 0)
                wrong = all_results[opt].get('wrong', 0)
                
                if baseline > 0:
                    print(f"{opt}: baseline={baseline:.6f}s, correct={correct:.6f}s ({correct/baseline:.3f}x), wrong={wrong:.6f}s ({wrong/baseline:.3f}x)")
    else:
        print("Не удалось получить результаты ни для одного уровня оптимизации")

if __name__ == "__main__":
    main() 