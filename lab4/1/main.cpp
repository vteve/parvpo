#include <iostream>
#include <fstream>
#include <random>
#include <chrono>
#include <string>
#include <sqlite3.h>
#include <cstring>

using namespace std;

// Функция для чтения строк из файла, которые содержат подстроку
void read_from_file_with_substring(const string& filename, int total_lines, const string& substring) {
    ifstream file_in(filename, ios::in);  // Открытие файла для чтения
    if (!file_in) {
        cerr << "Ошибка при открытии файла!" << endl;
        return;
    }

    string str;
    int length;
    auto start = chrono::high_resolution_clock::now();

    int found_count = 0;
    for (int i = 0; i < total_lines; ++i) {
        file_in >> length; // Считываем длину
        file_in >> ws;  // Пропускаем возможные пробелы
        getline(file_in, str); // Считываем строку

        if (strstr(str.c_str(), substring.c_str())) {
            ++found_count; // Если строка содержит подстроку, увеличиваем счетчик
        }
    }

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> duration = end - start;
    cout << "Время чтения строк с подстрокой из обычного файла: " << duration.count() << " секунд." << endl;
    cout << "Найдено строк с подстрокой: " << found_count << endl;

    file_in.close();
}

// Функция для чтения строк из SQLite3, которые содержат подстроку
void read_from_sqlite_with_substring(const string& db_name, int total_lines, const string& substring) {
    sqlite3* db;
    sqlite3_stmt* stmt;
    char* err_msg = nullptr;

    if (sqlite3_open(db_name.c_str(), &db)) {
        cerr << "Ошибка открытия базы данных: " << sqlite3_errmsg(db) << endl;
        return;
    }

    string query = "SELECT length, data FROM strings WHERE data LIKE '%" + substring + "%' LIMIT ?";
    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        cerr << "Ошибка при подготовке запроса: " << sqlite3_errmsg(db) << endl;
        sqlite3_close(db);
        return;
    }

    sqlite3_bind_int(stmt, 1, total_lines);  // Ограничиваем количество строк

    auto start = chrono::high_resolution_clock::now();

    int found_count = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        const char* data = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        if (strstr(data, substring.c_str())) {
            ++found_count; // Если строка содержит подстроку, увеличиваем счетчик
        }
    }

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> duration = end - start;
    cout << "Время чтения строк с подстрокой из SQLite3: " << duration.count() << " секунд." << endl;
    cout << "Найдено строк с подстрокой: " << found_count << endl;

    sqlite3_finalize(stmt);
    sqlite3_close(db);
}

int main(){
    int total_lines = 3000000; 
    string substring = "ab";
    read_from_file_with_substring("../write-on-file/data.txt", total_lines, substring);

    read_from_sqlite_with_substring("../write-on-database/database.db", total_lines, substring);

    read_from_sqlite_with_substring("../write-on-database/database.db", total_lines, substring);

    read_from_file_with_substring("../write-on-file/data.txt", total_lines, substring);
    return 0;
}