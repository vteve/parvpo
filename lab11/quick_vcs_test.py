#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import os
import subprocess
import time
import random
import json
import shutil
from pathlib import Path

class QuickVCSTest:
    def __init__(self, base_repo_dir="test_repo"):
        self.base_repo_dir = Path(base_repo_dir)
        self.results = {}

    def _rmtree_onerror(self, func, path, exc_info):
        import stat
        if not isinstance(exc_info[1], PermissionError):
            raise
        try:
            os.chmod(path, stat.S_IWRITE)
            func(path)
        except Exception as e:
            print(f"Failed to delete {path} after changing permissions: {e}")
            raise

    def run_command(self, cmd, cwd, capture_output=True):
        """Выполняет команду и возвращает результат"""
        try:
            return subprocess.run(
                cmd, shell=True, cwd=cwd, capture_output=capture_output, 
                text=True, encoding='utf-8', check=False
            )
        except Exception as e:
            print(f"Ошибка выполнения команды '{cmd}': {e}")
            return None

    def test_simple_git(self):
        """Простой тест Git без конфликтов"""
        print("\n" + "="*50)
        print("ПРОСТОЙ ТЕСТ GIT")
        print("="*50)
        
        git_dir = Path("git_quick_test_repo").resolve()
        if git_dir.exists():
            shutil.rmtree(git_dir, onerror=self._rmtree_onerror)
        
        print("Копируем файлы для Git...")
        shutil.copytree(self.base_repo_dir, git_dir)
        
        print("Инициализируем Git репозиторий...")
        self.run_command("git init -b main", cwd=git_dir)
        
        print("Добавляем все файлы...")
        start_time = time.time()
        self.run_command("git add .", cwd=git_dir)
        self.run_command('git commit -m "Initial commit"', cwd=git_dir)
        add_time = time.time() - start_time
        
        print("Изменяем половину файлов...")
        files = [f for f in git_dir.rglob("*") if f.is_file() and ".git" not in f.parts]
        files_to_modify = random.sample(files, len(files) // 2)
        
        start_time = time.time()
        for file_path in files_to_modify:
            with open(file_path, 'a', encoding='utf-8') as f:
                f.write(f"\n# Modified at {time.time()}\n")
        
        self.run_command('git commit -a -m "Modify half of files"', cwd=git_dir)
        modify_time = time.time() - start_time
        
        print("Создаем ветку 'feature'...")
        self.run_command("git checkout -b feature", cwd=git_dir)
        
        remaining_files = [f for f in files if f not in files_to_modify]
        files_to_modify_in_branch = random.sample(remaining_files, len(remaining_files) // 2)
        
        for file_path in files_to_modify_in_branch:
            with open(file_path, 'a', encoding='utf-8') as f:
                f.write(f"\n# Modified in branch at {time.time()}\n")
        
        self.run_command('git commit -a -m "Modify files in branch"', cwd=git_dir)
        
        print("Возвращаемся в ветку 'main' и выполняем слияние...")
        self.run_command("git checkout main", cwd=git_dir)
        merge_start = time.time()
        merge_result = self.run_command("git merge feature", cwd=git_dir)
        merge_time = time.time() - merge_start
        
        self.results['git_simple'] = {
            'add_time': add_time,
            'modify_time': modify_time,
            'merge_time': merge_time,
            'success': merge_result is not None and merge_result.returncode == 0
        }
        
        print(f"Git (простой тест) результаты:")
        print(f"  Добавление: {add_time:.2f}с")
        print(f"  Изменение: {modify_time:.2f}с")
        print(f"  Слияние: {merge_time:.2f}с")
        print(f"  Успех слияния: {self.results['git_simple']['success']}")
    
    def save_results(self):
        """Сохраняет результаты в JSON"""
        with open('vcs_quick_test_results.json', 'w', encoding='utf-8') as f:
            json.dump(self.results, f, indent=2, ensure_ascii=False)
        print("\nРезультаты сохранены в vcs_quick_test_results.json")
    
    def print_summary(self):
        """Выводит сводку результатов"""
        print("\n" + "="*60)
        print("СВОДКА РЕЗУЛЬТАТОВ БЫСТРОГО ТЕСТА")
        print("="*60)
        
        if not self.results:
            print("Нет результатов для анализа")
            return
        
        print(f"{'Система':<20} {'Добавление':<12} {'Изменение':<12} {'Слияние':<12} {'Успех':<8}")
        print("-" * 70)
        
        for vcs, data in self.results.items():
            success = "✓" if data.get('success') else "✗"
            print(f"{vcs:<20} {data.get('add_time', 0):<12.2f} {data.get('modify_time', 0):<12.2f} {data.get('merge_time', 0):<12.2f} {success:<8}")

def main():
    print("=" * 70)
    print("БЫСТРЫЙ ТЕСТ СИСТЕМЫ КОНТРОЛЯ ВЕРСИЙ (GIT)")
    print("=" * 70)
    
    if not os.path.exists("test_repo"):
        print("Директория 'test_repo' не найдена! Сначала запустите generate_files.py")
        return
    
    test = QuickVCSTest()
    
    if shutil.which('git'):
        test.test_simple_git()
    else:
        print("Git не найден в системе. Тест пропущен.")
    
    if not test.results:
        print("\nТест не был выполнен. Проверьте, установлена ли система Git.")
        return
        
    test.print_summary()
    test.save_results()
    
    print("\nТест завершен!")
    print("\nДля полного бенчмарка (Git, Mercurial & Subversion) запустите:")
    print("python vcs_benchmark.py")

if __name__ == "__main__":
    main() 