#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <cstdlib>
#include <windows.h>

#if (defined(__GNUC__) && (__GNUC__ >= 3)) || (defined(__INTEL_COMPILER)) || defined(__clang__)
    #define likely(expr)   (__builtin_expect(static_cast<bool>(expr), true))
    #define unlikely(expr) (__builtin_expect(static_cast<bool>(expr), false))
#else
    #define likely(expr)   (expr)
    #define unlikely(expr) (expr)
#endif

using namespace std;
using namespace std::chrono;

static const size_t N = 500'000'000;
static const int REPEATS = 3;


void fill_array(vector<int>& a) {
    std::mt19937_64 rng(42);
    std::uniform_int_distribution<int> dist(1, 1000);
    for (size_t i = 0; i < a.size(); ++i) {
        a[i] = dist(rng);
    }
}


void baseline_sum(const vector<int>& a, long long& sum_many, long long& sum_rare) {
    sum_many = 0;
    sum_rare = 0;
    for (size_t i = 0; i < a.size(); ++i) {
        if (i % 1000 == 0) {
            sum_rare += a[i];
        } else {
            sum_many += a[i];
        }
    }
}

void correct_hint_sum(const vector<int>& a, long long& sum_many, long long& sum_rare) {
    sum_many = 0;
    sum_rare = 0;
    for (size_t i = 0; i < a.size(); ++i) {
        if (unlikely(i % 1000 == 0)) {
            sum_rare += a[i];
        } else {
            sum_many += a[i];
        }
    }
}


void wrong_hint_sum(const vector<int>& a, long long& sum_many, long long& sum_rare) {
    sum_many = 0;
    sum_rare = 0;
    for (size_t i = 0; i < a.size(); ++i) {
        if (likely(i % 1000 == 0)) {
            sum_rare += a[i];
        } else {
            sum_many += a[i];
        }
    }
}


void inverted_hint_sum(const vector<int>& a, long long& sum_many, long long& sum_rare) {
    sum_many = 0;
    sum_rare = 0;
    for (size_t i = 0; i < a.size(); ++i) {
        if (!(unlikely(i % 1000 != 0))) {
            sum_rare += a[i];
        } else {
            sum_many += a[i];
        }
    }
}


template<typename Func>
double measure_time(Func f, const vector<int>& a, long long& out_many, long long& out_rare) {
    double total = 0.0;
    long long sm = 0, sr = 0;
    for (int r = 0; r < REPEATS; ++r) {
        auto start = high_resolution_clock::now();
        f(a, sm, sr);
        auto end = high_resolution_clock::now();
        total += duration<double>(end - start).count();
    }
    out_many = sm;
    out_rare = sr;
    return total / REPEATS;
}


int main() {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    cout << "=======================================================\n\n";
    vector<int> data(N);
    fill_array(data);
    cout << "Размер массива " << N << " элементов\n";
    long long baseline_many = 0, baseline_rare = 0;
    long long correct_many  = 0, correct_rare  = 0;
    long long wrong_many    = 0, wrong_rare    = 0;
    long long invert_many   = 0, invert_rare   = 0;

    double t_baseline = measure_time(baseline_sum,       data, baseline_many, baseline_rare);
    double t_correct  = measure_time(correct_hint_sum,  data, correct_many,  correct_rare);
    double t_wrong    = measure_time(wrong_hint_sum,    data, wrong_many,    wrong_rare);
    double t_invert   = measure_time(inverted_hint_sum, data, invert_many,   invert_rare);

    cout << "=== Результаты (усреднённое время из " << REPEATS << " запусков) ===\n\n";
    cout << "1) Baseline (без подсказок):\n";
    cout << "   Time = " << t_baseline << " с,  sum_many = " << baseline_many
         << ", sum_rare = " << baseline_rare << "\n\n";
    cout << "2) Верная подсказка (unlikely(i%1000==0)):\n";
    cout << "   Time = " << t_correct << " с,  sum_many = " << correct_many
         << ", sum_rare = " << correct_rare << "\n\n";
    cout << "3) Неверная подсказка (likely(i%1000==0)):\n";
    cout << "   Time = " << t_wrong << " с,  sum_many = " << wrong_many
         << ", sum_rare = " << wrong_rare << "\n\n";
    cout << "4) Инвертированная «перевернутая» подсказка:\n";
    cout << "   Time = " << t_invert << " с,  sum_many = " << invert_many
         << ", sum_rare = " << invert_rare << "\n\n";
    cout << "=======================================================\n";
    return 0;
}