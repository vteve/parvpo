#include <sqlite3.h>
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
    uniform_int_distribution<> dis(48, 57);  // Для цифр
    uniform_int_distribution<> dis_upper(65, 90);  // Для заглавных букв
    uniform_int_distribution<> dis_lower(97, 122);  // Для строчных букв
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

// Функция для записи строк в SQLite3
chrono::duration<double>  write_to_sqlite(const string& db_name, int total_lines, int max_length) {
    chrono::duration<double> duration(0);
    sqlite3* db;
    char* err_msg = nullptr;
    auto start = chrono::high_resolution_clock::now();
    if (sqlite3_open(db_name.c_str(), &db)) {
        cerr << "Ошибка открытия базы данных: " << sqlite3_errmsg(db) << endl;
        return duration;
    }
    const char* create_table_sql = "CREATE TABLE IF NOT EXISTS strings (id INTEGER PRIMARY KEY, length INTEGER, data TEXT);";
    if (sqlite3_exec(db, create_table_sql, nullptr, nullptr, &err_msg) != SQLITE_OK) {
        cerr << "Ошибка при создании таблицы: " << err_msg << endl;
        sqlite3_free(err_msg);
        sqlite3_close(db);
        return duration;
    }
    auto end = chrono::high_resolution_clock::now();
    duration += end - start;
    // Вставляем данные в таблицу
    sqlite3_exec(db, "BEGIN TRANSACTION;", nullptr, nullptr, nullptr); // Начинаем транзакцию
    for (int i = 0; i < total_lines; ++i) {
        int length = rand() % (max_length + 1);
        string str = generate_random_string(length);
        start = chrono::high_resolution_clock::now();
        // Подготовка SQL-запроса
        string insert_sql = "INSERT INTO strings (length, data) VALUES (" + to_string(length) + ", '" + str + "');";
        if (sqlite3_exec(db, insert_sql.c_str(), nullptr, nullptr, &err_msg) != SQLITE_OK) {
            cerr << "Ошибка при вставке данных: " << err_msg << endl;
            sqlite3_free(err_msg);
            sqlite3_close(db);
            return duration;
        }
        end = chrono::high_resolution_clock::now();
        duration += end - start;
    }
    start = chrono::high_resolution_clock::now();
    sqlite3_exec(db, "COMMIT;", nullptr, nullptr, nullptr); // Завершаем транзакцию
    sqlite3_close(db); 
    end = chrono::high_resolution_clock::now();
    duration += end - start;
    return duration;
}

int main() {
    // 300 MB
    int total_lines = 3000000; 
    int max_length = 1000;      // k
    auto duration = write_to_sqlite("database.db", total_lines, max_length);
    cout << "Время записи в SQLite3: " << duration.count() << " секунд." << endl;
    return 0;
}