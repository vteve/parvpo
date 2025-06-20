#include <iostream>
#include <fstream>
#include <random>
#include <chrono>
#include <string>

using namespace std;

// Функция для генерации случайной строки, содержащей только латинские буквы и цифры
string generate_random_string(int length) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis('0', '9');  // Для цифр
    uniform_int_distribution<> dis_upper('A', 'Z');  // Для заглавных букв
    uniform_int_distribution<> dis_lower('a', 'z');  // Для строчных букв
    string result(length, ' ');
    for (int i = 0; i < length; ++i) {
        int rand_choice = rand() % 3;  // Выбираем случайно, что добавить в строку
        if (rand_choice == 0) {
            result[i] = static_cast<char>(dis(gen));  // Цифра
        } else if (rand_choice == 1) {
            result[i] = static_cast<char>(dis_upper(gen));  // Заглавная буква
        } else {
            result[i] = static_cast<char>(dis_lower(gen));  // Строчная буква
        }
    }
    return result;
}

chrono::duration<double> write_to_file(const string& filename, int total_lines, int max_length) {
    chrono::duration<double> duration(0);
    auto start = chrono::high_resolution_clock::now();
    ofstream file(filename, ios::out);
    if (!file) {
        cerr << "Ошибка открытия файла!" << endl;
        return duration;
    }
    auto end = chrono::high_resolution_clock::now();
    duration += end - start;
    for (int i = 0; i < total_lines; ++i) {
        int length = rand() % (max_length + 1);  
        string str = generate_random_string(length);
        start = chrono::high_resolution_clock::now();
        file << length << " " << str << endl;
        end = chrono::high_resolution_clock::now();
        duration += end - start;
    }
    start = chrono::high_resolution_clock::now();
    file.close();
    end = chrono::high_resolution_clock::now();
    duration += end - start;
    return duration;
}

int main() {
    // 300 MB
    int total_lines = 3000000;  
    int max_length = 1000;      
    //auto start = chrono::high_resolution_clock::now();
    auto duration = write_to_file("data.txt", total_lines, max_length);
    //auto end = chrono::high_resolution_clock::now();
    //chrono::duration<double> duration = end - start;
    cout << "Время записи в файл: " << duration.count() << " секунд." << endl;
    return 0;
}