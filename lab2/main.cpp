#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <numeric>
#include <random>
#include <windows.h>

constexpr int SEED = 42;

class RandomGenerator {
private:
    unsigned long a, c, m, state;
public:
    RandomGenerator(unsigned long seed, unsigned long a = 1664525, unsigned long c = 1013904223, unsigned long m = 4294967296)
        : a(a), c(c), m(m), state(seed) {}
    unsigned long next() {
        state = (a * state + c) % m;
        return state;
    }
};

void create_test_file(const std::string& filename, size_t size) {
    std::ofstream file_out(filename, std::ios::binary);
    if (!file_out) {
        std::cerr << "Ошибка при создании тестового файла!" << std::endl;
        return;
    }
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 255);
    
    std::vector<char> buffer(1024);
    size_t remaining = size;
    
    while (remaining > 0) {
        size_t to_write = std::min(remaining, buffer.size());
        for (size_t i = 0; i < to_write; ++i) {
            buffer[i] = static_cast<char>(dis(gen));
        }
        file_out.write(buffer.data(), to_write);
        remaining -= to_write;
    }
    
    file_out.close();
}

void measure_time(int k, std::ifstream& file_in, std::streamsize file_size) {
    const int block_size = 8; 
    const int num_reads = 1000000; 
    if (file_size < k) {
        std::cerr << "Размер k превышает размер файла! Пропускаем k = " << k << std::endl;
        return;
    }
    RandomGenerator rng(SEED); 
    unsigned long a = rng.next() % (file_size - k);
    std::cout << "Используем фиксированное смещение a = " << a << std::endl;
    auto start = std::chrono::high_resolution_clock::now();
    unsigned long sum = 0;
    for (int i = 0; i < num_reads; ++i) {
        unsigned long offset = a + (rng.next() % k);
        if (offset + block_size > file_size) {
            continue;
        }
        file_in.seekg(offset, std::ios::beg);
        char buffer[block_size];
        file_in.read(buffer, block_size);
        for (int j = 0; j < block_size; ++j) {
            sum += static_cast<unsigned char>(buffer[j]); 
        }
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    std::cout << "Time for k = " << k << ": " << duration.count() << " seconds, checksum = " << sum << std::endl;
}

int main() {
    // Установка кодировки консоли для Windows
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    
    const std::string filename = "test_file.bin";
    const size_t file_size = 100 * 1024 * 1024; // 100 MB
    
    // Create test file if it doesn't exist
    std::ifstream check_file(filename);
    if (!check_file.good()) {
        std::cout << "Создаем тестовый файл размером " << file_size << " байт..." << std::endl;
        create_test_file(filename, file_size);
    }
    check_file.close();
    
    std::ifstream file_in(filename, std::ios::binary | std::ios::ate);
    if (!file_in) {
        std::cerr << "Ошибка при открытии файла!" << std::endl;
        return 1;
    }
    
    std::streamsize actual_file_size = file_in.tellg();
    file_in.seekg(0, std::ios::beg);
    std::cout << "Размер файла: " << actual_file_size << " байт" << std::endl;
    
    std::vector<int> ks = {24000, 30000, 48000, 80000, 200000, 500000, 1000000, 
                           5000000, 8000000, 20000000, 40000000, 80000000}; 
    for (int k : ks) {
        measure_time(k, file_in, actual_file_size);
    }
    
    file_in.close();
    return 0;
}