// Рекомендуется перед запуском программы выполнить в консоли: chcp 65001
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <random>
#include <chrono>
#include <filesystem>
#include <cstdlib>
#include <clocale>

namespace fs = std::filesystem;

// --- Настройки генерации ---
const size_t TARGET_SIZE = 10000; // 10 КБ
const int MIN_PEOPLE = 6;
const int MAX_PEOPLE = 10;
const int ROW_SIZE = 5; // Людей в ряду

// --- Списки для генерации имён ---
const std::vector<std::string> first_names = {
    "Иван", "Петр", "Сергей", "Алексей", "Дмитрий", "Андрей", "Максим", "Егор", "Антон", "Владимир"
};
const std::vector<std::string> last_names = {
    "Иванов", "Петров", "Сидоров", "Кузнецов", "Смирнов", "Попов", "Васильев", "Новиков", "Федоров", "Морозов"
};
const std::vector<std::string> patronymics = {
    "Иванович", "Петрович", "Сергеевич", "Алексеевич", "Дмитриевич", "Андреевич", "Максимович", "Егорович", "Антонович", "Владимирович"
};

std::string random_name(std::mt19937& rng) {
    std::uniform_int_distribution<> f(0, first_names.size() - 1);
    std::uniform_int_distribution<> l(0, last_names.size() - 1);
    std::uniform_int_distribution<> p(0, patronymics.size() - 1);
    return last_names[l(rng)] + " " + first_names[f(rng)] + " " + patronymics[p(rng)];
}

int main() {
    std::setlocale(LC_ALL, "Russian");
    std::mt19937 rng(std::random_device{}());
    std::ofstream out("pretty_groups.pikchr");
    size_t total_bytes = 0;
    int group_count = 0;

    while (total_bytes < TARGET_SIZE) {
        ++group_count;
        out << "box \"Группа " << group_count << "\" fit\n";
        out << "down\n";
        std::uniform_int_distribution<> people_dist(MIN_PEOPLE, MAX_PEOPLE);
        int people = people_dist(rng);
        int in_row = 0;
        for (int i = 0; i < people; ++i) {
            if (i == 0) {
                out << "box \"" << random_name(rng) << "\" fit\n";
                in_row = 1;
            } else if (in_row < ROW_SIZE) {
                out << "right\n";
                out << "box \"" << random_name(rng) << "\" fit\n";
                ++in_row;
            } else {
                out << "down\n";
                out << "box \"" << random_name(rng) << "\" fit\n";
                in_row = 1;
            }
        }
        out << "\n"; // Пустая строка между группами
        out.flush();
        total_bytes = out.tellp();
    }
    out.close();

    // --- Замер времени генерации SVG ---
    auto start = std::chrono::high_resolution_clock::now();
    int ret = std::system("pikchr\\pikchr.exe --svg-only pretty_groups.pikchr > pretty_groups.svg");
    auto end = std::chrono::high_resolution_clock::now();

    if (ret != 0) {
        std::wcerr << L"Ошибка при запуске pikchr.exe\n";
        return 1;
    }

    // --- Подсчёт размеров файлов ---
    auto input_size = fs::file_size("pretty_groups.pikchr");
    auto output_size = fs::file_size("pretty_groups.svg");

    // --- Подсчёт времени ---
    auto duration_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    double duration_s = duration_ns / 1e9;

    // --- Подсчёт количества диаграмм (групп) ---
    std::wcout << L"Сгенерировано групп: " << group_count << std::endl;
    std::wcout << L"Размер входного файла: " << input_size << L" байт\n";
    std::wcout << L"Размер SVG-файла: " << output_size << L" байт\n";
    std::wcout << L"Соотношение вход/выход: " << (double)input_size / output_size << std::endl;
    std::wcout << L"Время генерации SVG: " << duration_s << L" сек\n";
    std::wcout << L"Наносекунд на байт входа: " << (double)duration_ns / input_size << L" нс\n";
    std::wcout << L"Микросекунд на одну диаграмму: " << (duration_ns / 1000.0) / group_count << L" мкс\n";
    return 0;
} 