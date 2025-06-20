#include <sqlite3.h>
#include <iostream>
#include <fstream>
#include <chrono>
#include <cstring>
#include <string>
#include <random>

using namespace std;

void read_from_file(const string& filename, int total_lines) {
    ifstream file(filename, ios::in);
    if (!file) {
        cerr << "Ошибка открытия файла!" << endl;
        return;
    }
    string str;
    int count = 0;
    auto start = chrono::high_resolution_clock::now();
    for (int i = 0; i < total_lines; ++i) {
        int length;
        file >> length; 
        file >> ws; 
        getline(file, str); 
        count++;
    }
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> duration = end - start;
    cout << "Время чтения строк из обычного файла: " << duration.count() << " секунд." << endl;
    cout << "Прочитано строк: " << count << endl;
    file.close();
}

void read_from_sqlite(const string& db_name, int total_lines) {
    sqlite3* db;
    char* err_msg = nullptr;
    if (sqlite3_open(db_name.c_str(), &db)) {
        cerr << "Ошибка открытия базы данных: " << sqlite3_errmsg(db) << endl;
        return;
    }
    const char* select_sql = "SELECT data FROM strings;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, select_sql, -1, &stmt, nullptr) != SQLITE_OK) {
        cerr << "Ошибка при подготовке запроса: " << sqlite3_errmsg(db) << endl;
        sqlite3_close(db);
        return;
    }
    auto start = chrono::high_resolution_clock::now();
    int count = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW && count < total_lines) {
        const char* str = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        count++;
    }
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> duration = end - start;
    cout << "Время чтения строк из SQLite3: " << duration.count() << " секунд." << endl;
    cout << "Прочитано строк: " << count << endl;
    sqlite3_finalize(stmt);
    sqlite3_close(db);
}

int main() {
    int total_lines = 3000000; 
    read_from_file("../write-on-file/data.txt", total_lines);  // Чтение из файла
    read_from_sqlite("../write-on-database/database.db", total_lines);  // Чтение из SQLite3
    read_from_sqlite("../write-on-database/database.db", total_lines);  // Чтение из SQLite3
    read_from_file("../write-on-file/data.txt", total_lines);  // Чтение из файла
    return 0;
}