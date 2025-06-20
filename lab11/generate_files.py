#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import os
import random
import string
import time
from pathlib import Path

def generate_random_content():
    """Генерирует случайное содержимое файла"""
    content_types = [
        # Python файлы
        lambda: f'''#!/usr/bin/env python3
# -*- coding: utf-8 -*-

def function_{random.randint(1000, 9999)}():
    """Функция {random.randint(1, 100)}"""
    value = {random.randint(1, 1000)}
    result = value * {random.randint(2, 10)}
    return result

if __name__ == "__main__":
    print(function_{random.randint(1000, 9999)}())
''',
        # C++ файлы
        lambda: f'''#include <iostream>
#include <vector>

int main() {{
    std::vector<int> data = {{{random.randint(1, 100)}, {random.randint(1, 100)}, {random.randint(1, 100)}}};
    int sum = 0;
    for (int x : data) {{
        sum += x;
    }}
    std::cout << "Sum: " << sum << std::endl;
    return 0;
}}
''',
        # JavaScript файлы
        lambda: f'''// JavaScript file {random.randint(1, 1000)}
const data = [{random.randint(1, 100)}, {random.randint(1, 100)}, {random.randint(1, 100)}];
const result = data.reduce((sum, x) => sum + x, 0);
console.log(`Result: ${{result}}`);
''',
        # HTML файлы
        lambda: f'''<!DOCTYPE html>
<html>
<head>
    <title>Page {random.randint(1, 1000)}</title>
</head>
<body>
    <h1>Welcome to page {random.randint(1, 1000)}</h1>
    <p>This is content {random.randint(1, 1000)}</p>
    <div>Value: {random.randint(1, 1000)}</div>
</body>
</html>
''',
        # JSON файлы
        lambda: f'''{{
    "id": {random.randint(1, 10000)},
    "name": "item_{random.randint(1, 1000)}",
    "value": {random.randint(1, 1000)},
    "data": [{random.randint(1, 100)}, {random.randint(1, 100)}, {random.randint(1, 100)}]
}}
''',
        # Markdown файлы
        lambda: f'''# Document {random.randint(1, 1000)}

This is document number {random.randint(1, 1000)}.

## Section {random.randint(1, 100)}

Content with value {random.randint(1, 1000)}.

- Item {random.randint(1, 100)}
- Item {random.randint(1, 100)}
- Item {random.randint(1, 100)}
''',
        # Text файлы
        lambda: f'''Text file {random.randint(1, 10000)}
Content line {random.randint(1, 1000)}
Another line with value {random.randint(1, 1000)}
Final line {random.randint(1, 1000)}
''',
        # CSS файлы
        lambda: f'''/* CSS file {random.randint(1, 1000)} */
.container {{
    width: {random.randint(100, 1000)}px;
    height: {random.randint(100, 1000)}px;
    margin: {random.randint(1, 50)}px;
    padding: {random.randint(1, 50)}px;
}}

.element {{
    color: #{random.randint(100000, 999999)};
    font-size: {random.randint(12, 48)}px;
}}
''',
        # Shell скрипты
        lambda: f'''#!/bin/bash
# Script {random.randint(1, 1000)}

echo "Running script {random.randint(1, 1000)}"
value={random.randint(1, 1000)}
echo "Value: $value"

for i in {{1..{random.randint(3, 10)}}}; do
    echo "Iteration $i"
done
''',
        # XML файлы
        lambda: f'''<?xml version="1.0" encoding="UTF-8"?>
<root>
    <item id="{random.randint(1, 10000)}">
        <name>item_{random.randint(1, 1000)}</name>
        <value>{random.randint(1, 1000)}</value>
        <data>{random.randint(1, 100)}</data>
    </item>
</root>
'''
    ]
    
    return random.choice(content_types)()

def generate_filename():
    """Генерирует случайное имя файла"""
    extensions = ['.py', '.cpp', '.js', '.html', '.json', '.md', '.txt', '.css', '.sh', '.xml']
    name_length = random.randint(5, 15)
    name = ''.join(random.choices(string.ascii_lowercase + string.digits, k=name_length))
    ext = random.choice(extensions)
    return f"{name}{ext}"

def create_directory_structure(base_dir, num_files=10000):
    """Создает структуру директорий и файлов"""
    print(f"Создаем {num_files} файлов в {base_dir}...")
    
    # Создаем базовую директорию
    base_path = Path(base_dir)
    base_path.mkdir(exist_ok=True)
    
    # Создаем поддиректории для лучшей организации
    subdirs = ['src', 'docs', 'tests', 'data', 'config', 'scripts', 'assets', 'lib']
    for subdir in subdirs:
        (base_path / subdir).mkdir(exist_ok=True)
    
    start_time = time.time()
    
    for i in range(num_files):
        # Выбираем случайную поддиректорию
        subdir = random.choice(subdirs)
        filename = generate_filename()
        filepath = base_path / subdir / filename
        
        # Генерируем содержимое
        content = generate_random_content()
        
        # Записываем файл
        with open(filepath, 'w', encoding='utf-8') as f:
            f.write(content)
        
        # Прогресс каждые 1000 файлов
        if (i + 1) % 1000 == 0:
            elapsed = time.time() - start_time
            print(f"Создано {i + 1} файлов за {elapsed:.2f} секунд")
    
    total_time = time.time() - start_time
    print(f"Готово! Создано {num_files} файлов за {total_time:.2f} секунд")
    
    # Статистика
    print("\nСтатистика по директориям:")
    for subdir in subdirs:
        count = len(list((base_path / subdir).glob('*')))
        print(f"  {subdir}: {count} файлов")

def main():
    print("=" * 60)
    print("ГЕНЕРАТОР ФАЙЛОВ ДЛЯ ТЕСТИРОВАНИЯ СИСТЕМ КОНТРОЛЯ ВЕРСИЙ")
    print("=" * 60)
    
    base_dir = "test_repo"
    num_files = 10000
    
    if os.path.exists(base_dir):
        print(f"Директория {base_dir} уже существует!")
        response = input("Удалить и создать заново? (y/N): ")
        if response.lower() == 'y':
            import shutil
            import stat

            def rmtree_onerror(func, path, exc_info):
                """
                Error handler for ``shutil.rmtree`` that attempts to fix permissions.
                """
                if not isinstance(exc_info[1], PermissionError):
                    raise
                try:
                    os.chmod(path, stat.S_IWRITE)
                    func(path)
                except Exception as e:
                    print(f"Failed to delete {path} after changing permissions: {e}")
                    raise
            
            try:
                print(f"Удаляем директорию {base_dir}...")
                shutil.rmtree(base_dir, onerror=rmtree_onerror)
                print(f"Удалена директория {base_dir}")
            except Exception as e:
                print(f"Не удалось удалить директорию {base_dir}. Ошибка: {e}")
                print("Пожалуйста, удалите папку 'test_repo' вручную и запустите скрипт снова.")
                return
        else:
            print("Операция отменена")
            return
    
    create_directory_structure(base_dir, num_files)
    
    print(f"\nФайлы созданы в директории: {base_dir}")
    print("Теперь можно инициализировать системы контроля версий")

if __name__ == "__main__":
    main() 