import os
import subprocess
import time
import random
import json
import shutil
from pathlib import Path
import matplotlib.pyplot as plt
import numpy as np

class VCSBenchmark:
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

    def test_git(self):
        """Тестирует Git"""
        print("\n" + "="*50)
        print("ТЕСТИРОВАНИЕ GIT")
        print("="*50)
        
        git_dir = Path("git_benchmark_repo").resolve()
        if git_dir.exists():
            shutil.rmtree(git_dir, onerror=self._rmtree_onerror)
        
        print("Копируем файлы для Git...")
        shutil.copytree(self.base_repo_dir, git_dir)
        
        print("Инициализируем Git репозиторий...")
        self.run_command("git init -b main", cwd=git_dir)
        
        print("Добавляем все файлы в Git...")
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
                f.write(f"\n# Modified in main at {time.time()}\n")
        
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
        
        self.results['git'] = {
            'add_time': add_time,
            'modify_time': modify_time,
            'merge_time': merge_time,
            'success': merge_result is not None and merge_result.returncode == 0
        }
        
        print(f"Git результаты:")
        print(f"  Добавление: {add_time:.2f}с")
        print(f"  Изменение: {modify_time:.2f}с")
        print(f"  Слияние: {merge_time:.2f}с")
        print(f"  Успех слияния: {self.results['git']['success']}")

    def test_mercurial(self):
        """Тестирует Mercurial (hg)"""
        print("\n" + "="*50)
        print("ТЕСТИРОВАНИЕ MERCURIAL (HG)")
        print("="*50)
        
        hg_dir = Path("mercurial_benchmark_repo")
        if hg_dir.exists():
            shutil.rmtree(hg_dir, onerror=self._rmtree_onerror)
        
        print("Копируем файлы для Mercurial...")
        shutil.copytree(self.base_repo_dir, hg_dir)
        
        print("Инициализируем Mercurial репозиторий...")
        self.run_command("hg init", cwd=hg_dir)
        
        print("Добавляем все файлы в Mercurial...")
        start_time = time.time()
        self.run_command("hg add", cwd=hg_dir)
        self.run_command('hg commit -m "Initial commit"', cwd=hg_dir)
        add_time = time.time() - start_time
        
        print("Изменяем половину файлов...")
        files = [f for f in hg_dir.rglob("*") if f.is_file() and ".hg" not in f.parts]
        files_to_modify = random.sample(files, len(files) // 2)
        
        start_time = time.time()
        for file_path in files_to_modify:
            with open(file_path, 'a', encoding='utf-8') as f:
                f.write(f"\n# Modified at {time.time()}\n")
        
        self.run_command('hg commit -m "Modify half of files"', cwd=hg_dir)
        modify_time = time.time() - start_time
        
        print("Создаем новую ветку 'feature'...")
        self.run_command("hg branch feature", cwd=hg_dir)
        
        remaining_files = [f for f in files if f not in files_to_modify]
        files_to_modify_branch = random.sample(remaining_files, len(remaining_files) // 2)
        
        for file_path in files_to_modify_branch:
            with open(file_path, 'a', encoding='utf-8') as f:
                f.write(f"\n# Modified in branch at {time.time()}\n")
        
        self.run_command('hg commit -m "Modify files in branch"', cwd=hg_dir)
        
        print("Возвращаемся в ветку 'default' и создаем конфликт...")
        self.run_command("hg checkout default", cwd=hg_dir)
        for file_path in files_to_modify_branch:
            with open(file_path, 'a', encoding='utf-8') as f:
                f.write(f"\n# Modified in default at {time.time()}\n")
        
        self.run_command('hg commit -m "Modify same files in default"', cwd=hg_dir)
        
        print("Выполняем слияние...")
        merge_start = time.time()
        merge_result = self.run_command("hg merge feature --tool internal:merge", cwd=hg_dir)
        if merge_result is None: # Handle potential failure
            self.results['mercurial'] = {'add_time': 0, 'modify_time': 0, 'merge_time': 0, 'success': False}
            print("Ошибка при слиянии Mercurial, тест прерван.")
            return

        self.run_command('hg commit -m "Merge branch"', cwd=hg_dir)
        merge_time = time.time() - merge_start
        
        self.results['mercurial'] = {
            'add_time': add_time,
            'modify_time': modify_time,
            'merge_time': merge_time,
            'success': "0 files updated, 0 files merged, 0 files removed, 0 files unresolved" not in (merge_result.stderr or "")
        }
        
        print(f"Mercurial результаты:")
        print(f"  Добавление: {add_time:.2f}с")
        print(f"  Изменение: {modify_time:.2f}с")
        print(f"  Слияние: {merge_time:.2f}с")
        print(f"  Успех слияния: {self.results['mercurial']['success']}")
    
    def test_svn(self):
        """Тестирует Subversion (SVN)"""
        print("\n" + "="*50)
        print("ТЕСТИРОВАНИЕ SUBVERSION (SVN)")
        print("="*50)
        
        # Use absolute paths for robustness on Windows
        svn_repo_path = Path("svn_server_repo").resolve()
        if svn_repo_path.exists():
            shutil.rmtree(svn_repo_path, onerror=self._rmtree_onerror)
        
        print("Создаем SVN репозиторий...")
        self.run_command(f'svnadmin create "{svn_repo_path}"', cwd=".")

        working_copy = Path("svn_working_copy").resolve()
        if working_copy.exists():
            shutil.rmtree(working_copy, onerror=self._rmtree_onerror)
        
        shutil.copytree(self.base_repo_dir, working_copy)
        
        repo_url = svn_repo_path.as_uri()

        print("Импортируем файлы в SVN...")
        start_time = time.time()
        self.run_command(f'svn import "{working_copy}" "{repo_url}/trunk" -m "Initial import"', cwd=".")
        add_time = time.time() - start_time
        
        shutil.rmtree(working_copy, onerror=self._rmtree_onerror)
        self.run_command(f'svn checkout "{repo_url}/trunk" "{working_copy}"', cwd=".")
        
        print("Изменяем половину файлов...")
        files = [f for f in working_copy.rglob("*") if f.is_file() and ".svn" not in f.parts]
        files_to_modify = random.sample(files, len(files) // 2)
        
        start_time = time.time()
        for file_path in files_to_modify:
            with open(file_path, 'a', encoding='utf-8') as f:
                f.write(f"\n# Modified at {time.time()}\n")
        
        self.run_command('svn commit -m "Modify half of files"', cwd=working_copy)
        modify_time = time.time() - start_time
        
        print("Создаем ветку...")
        self.run_command(f'svn copy "{repo_url}/trunk" "{repo_url}/branches/feature" -m "Create branch"', cwd=".")
        
        branch_wc = Path("svn_branch_wc").resolve()
        if branch_wc.exists():
            shutil.rmtree(branch_wc, onerror=self._rmtree_onerror)
        self.run_command(f'svn checkout "{repo_url}/branches/feature" "{branch_wc}"', cwd=".")
        
        remaining_files = [f for f in files if f not in files_to_modify]
        files_to_modify_branch = random.sample(remaining_files, len(remaining_files) // 2)
        
        for file_path in files_to_modify_branch:
            relative_p = file_path.relative_to(working_copy)
            branch_file = branch_wc / relative_p
            if branch_file.exists():
                with open(branch_file, 'a', encoding='utf-8') as f:
                    f.write(f"\n# Modified in branch at {time.time()}\n")

        self.run_command('svn commit -m "Modify files in branch"', cwd=branch_wc)
        
        for file_path in files_to_modify_branch:
            with open(file_path, 'a', encoding='utf-8') as f:
                f.write(f"\n# Modified in trunk at {time.time()}\n")
        
        self.run_command('svn commit -m "Modify same files in trunk"', cwd=working_copy)
        
        print("Выполняем слияние...")
        merge_start = time.time()
        merge_result = self.run_command(f'svn merge "{repo_url}/branches/feature"', cwd=working_copy)
        if merge_result is None:
            self.results['subversion'] = {'add_time': 0, 'modify_time': 0, 'merge_time': 0, 'success': False}
            print("Ошибка при слиянии Subversion, тест прерван.")
            return

        self.run_command('svn commit -m "Merge branch"', cwd=working_copy)
        merge_time = time.time() - merge_start
        
        self.results['subversion'] = {
            'add_time': add_time,
            'modify_time': modify_time,
            'merge_time': merge_time,
            'success': "conflict" not in (merge_result.stdout or "").lower()
        }
        
        print(f"Subversion результаты:")
        print(f"  Добавление: {add_time:.2f}с")
        print(f"  Изменение: {modify_time:.2f}с")
        print(f"  Слияние: {merge_time:.2f}с")
        print(f"  Успех слияния: {self.results['subversion']['success']}")

    def create_charts(self):
        """Создает графики сравнения"""
        if not self.results:
            print("Нет результатов для создания графиков")
            return
        
        vcs_names = list(self.results.keys())
        operations = ['add_time', 'modify_time', 'merge_time']
        
        fig, axes = plt.subplots(1, 3, figsize=(15, 5))
        
        for i, operation in enumerate(operations):
            values = [self.results[vcs][operation] for vcs in vcs_names]
            
            axes[i].bar(vcs_names, values, alpha=0.8)
            axes[i].set_title(f'{operation.replace("_", " ").title()}')
            axes[i].set_ylabel('Время (секунды)')
            axes[i].tick_params(axis='x', rotation=45)
            
            # Добавляем значения на столбцы
            for j, v in enumerate(values):
                axes[i].text(j, v + max(values) * 0.01, f'{v:.2f}s', 
                           ha='center', va='bottom')
        
        plt.tight_layout()
        plt.savefig('vcs_benchmark_results.png', dpi=300, bbox_inches='tight')
        print("График сохранен в vcs_benchmark_results.png")
        plt.show()
    
    def save_results(self):
        """Сохраняет результаты в JSON"""
        with open('vcs_benchmark_results.json', 'w', encoding='utf-8') as f:
            json.dump(self.results, f, indent=2, ensure_ascii=False)
        print("Результаты сохранены в vcs_benchmark_results.json")
    
    def print_summary(self):
        """Выводит сводку результатов"""
        print("\n" + "="*60)
        print("СВОДКА РЕЗУЛЬТАТОВ")
        print("="*60)
        
        if not self.results:
            print("Нет результатов для анализа")
            return
        
        # Таблица результатов
        print(f"{'Система':<15} {'Добавление':<12} {'Изменение':<12} {'Слияние':<12} {'Успех':<8}")
        print("-" * 70)
        
        for vcs, data in self.results.items():
            success = "✓" if data['success'] else "✗"
            print(f"{vcs:<15} {data['add_time']:<12.2f} {data['modify_time']:<12.2f} {data['merge_time']:<12.2f} {success:<8}")
        
        # Анализ
        print("\nАНАЛИЗ:")
        print("-" * 30)
        
        for operation in ['add_time', 'modify_time', 'merge_time']:
            best_vcs = min(self.results.keys(), key=lambda x: self.results[x][operation])
            worst_vcs = max(self.results.keys(), key=lambda x: self.results[x][operation])
            
            best_time = self.results[best_vcs][operation]
            worst_time = self.results[worst_vcs][operation]
            
            print(f"{operation.replace('_', ' ').title()}:")
            print(f"  Лучший: {best_vcs} ({best_time:.2f}с)")
            print(f"  Худший: {worst_vcs} ({worst_time:.2f}с)")
            if worst_time > 0:
                ratio = worst_time / best_time
                print(f"  Разница: {ratio:.1f}x")
            print()

def main():
    print("=" * 70)
    print("БЕНЧМАРК СИСТЕМ КОНТРОЛЯ ВЕРСИЙ (GIT, MERCURIAL & SUBVERSION)")
    print("=" * 70)
    
    if not os.path.exists("test_repo"):
        print("Директория 'test_repo' не найдена! Сначала запустите generate_files.py")
        return
    
    benchmark = VCSBenchmark()
    
    if shutil.which('git'):
        benchmark.test_git()
    else:
        print("\nGit не найден в системе. Пропускаем.")

    if shutil.which('hg'):
        benchmark.test_mercurial()
    else:
        print("\nMercurial (hg) не найден в системе. Пропускаем.")
    
    if shutil.which('svn') and shutil.which('svnadmin'):
        benchmark.test_svn()
    else:
        print("\nSubversion (svn/svnadmin) не найден в системе. Пропускаем.")
    
    if not benchmark.results:
        print("\nНе было протестировано ни одной системы контроля версий. Бенчмарк прерван.")
        return

    benchmark.print_summary()
    benchmark.save_results()
    
    try:
        import matplotlib
        benchmark.create_charts()
    except ImportError:
        print("\nmatplotlib не установлен, графики не созданы. Установите: pip install matplotlib")
    
    print("\nБенчмарк завершен!")

if __name__ == "__main__":
    main() 