#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <cmath>
#include <cassert>
#include <clocale>
#include <windows.h>

#ifdef _OPENMP
  #include <omp.h>
#endif

using namespace std;
using clk = chrono::high_resolution_clock;

static constexpr size_t N = 50'000'000;


void experiment_quadratic()
{
    cout << "=== Часть A: решение квадратных уравнений ===\n\n";

    vector<double> A(N), B(N), C(N);
    {
        mt19937_64 rng(42);
        uniform_real_distribution<double> dist(-1000.0, 1000.0);
        for (size_t i = 0; i < N; ++i) {
            double a = dist(rng);
            while (fabs(a) < 1e-9) {
                a = dist(rng);
            }
            A[i] = a;
            B[i] = dist(rng);
            C[i] = dist(rng);
        }
    }

    enum Mode { 
        BASE = 0,      // все указатели «обычные»
        CONST_ALL,     // входные указатели const double *
        VOLATILE_ALL,  // входные указатели volatile double *
        RESTRICT_ALL   // входные указатели double * __restrict__
    };

    size_t loop_bound_var   = N;       // «неявная» переменная
    const size_t loop_bound_c = N;     // «явно константная»

    auto run_variant = [&]( const string &name,
                            double *a_arr_raw,
                            double *b_arr_raw,
                            double *c_arr_raw,
                            bool    use_const_bound,
                            bool    with_openmp )
    {
        Mode mode = BASE;
        if      (name.rfind("BASELINE",    0) == 0) mode = BASE;
        else if (name.rfind("CONST_ALL",   0) == 0) mode = CONST_ALL;
        else if (name.rfind("VOLATILE_ALL",0) == 0) mode = VOLATILE_ALL;
        else if (name.rfind("RESTRICT_ALL",0) == 0) mode = RESTRICT_ALL;
        else {
            assert(false && "run_variant: неизвестный режим");
        }

        double *        a_arr      = nullptr;
        double *        b_arr      = nullptr;
        double *        c_arr      = nullptr;
        const double *  a_arr_c    = nullptr;
        const double *  b_arr_c    = nullptr;
        const double *  c_arr_c    = nullptr;
        volatile double *a_arr_v   = nullptr;
        volatile double *b_arr_v   = nullptr;
        volatile double *c_arr_v   = nullptr;
        double * __restrict__ a_arr_r = nullptr;
        double * __restrict__ b_arr_r = nullptr;
        double * __restrict__ c_arr_r = nullptr;

        switch (mode) {
            case BASE:
                a_arr = a_arr_raw;
                b_arr = b_arr_raw;
                c_arr = c_arr_raw;
                break;
            case CONST_ALL:
                a_arr_c = a_arr_raw;
                b_arr_c = b_arr_raw;
                c_arr_c = c_arr_raw;
                break;
            case VOLATILE_ALL:
                a_arr_v = reinterpret_cast<volatile double*>(a_arr_raw);
                b_arr_v = reinterpret_cast<volatile double*>(b_arr_raw);
                c_arr_v = reinterpret_cast<volatile double*>(c_arr_raw);
                break;
            case RESTRICT_ALL:
                a_arr_r = a_arr_raw;
                b_arr_r = b_arr_raw;
                c_arr_r = c_arr_raw;
                break;
        }

        size_t roots_count = 0u;
        auto t_start = clk::now();

        if (with_openmp)
        {
            #ifdef _OPENMP
              int max_thr = omp_get_max_threads();
            #else
              int max_thr = 1;
            #endif

            if (mode == BASE) {
                size_t lb = (use_const_bound ? loop_bound_c : loop_bound_var);
                #pragma omp parallel for reduction(+:roots_count)
                for (size_t i = 0; i < lb; ++i) {
                    double a = a_arr[i];
                    double b = b_arr[i];
                    double c = c_arr[i];
                    double D = b*b - 4.0*a*c;
                    if (D >= 0.0) {
                        double sd = sqrt(D);
                        double x1 = (-b + sd)/(2.0*a);
                        double x2 = (-b - sd)/(2.0*a);
                        (void)x1; (void)x2;
                        roots_count += 2;
                    }
                }
            }
            else if (mode == CONST_ALL) {
                size_t lb = (use_const_bound ? loop_bound_c : loop_bound_var);
                #pragma omp parallel for reduction(+:roots_count)
                for (size_t i = 0; i < lb; ++i) {
                    double a = a_arr_c[i];
                    double b = b_arr_c[i];
                    double c = c_arr_c[i];
                    double D = b*b - 4.0*a*c;
                    if (D >= 0.0) {
                        double sd = sqrt(D);
                        double x1 = (-b + sd)/(2.0*a);
                        double x2 = (-b - sd)/(2.0*a);
                        (void)x1; (void)x2;
                        roots_count += 2;
                    }
                }
            }
            else if (mode == VOLATILE_ALL) {
                size_t lb = (use_const_bound ? loop_bound_c : loop_bound_var);
                #pragma omp parallel for reduction(+:roots_count)
                for (size_t i = 0; i < lb; ++i) {
                    double a = a_arr_v[i];
                    double b = b_arr_v[i];
                    double c = c_arr_v[i];
                    double D = b*b - 4.0*a*c;
                    if (D >= 0.0) {
                        double sd = sqrt(D);
                        double x1 = (-b + sd)/(2.0*a);
                        double x2 = (-b - sd)/(2.0*a);
                        (void)x1; (void)x2;
                        roots_count += 2;
                    }
                }
            }
            else if (mode == RESTRICT_ALL) {
                size_t lb = (use_const_bound ? loop_bound_c : loop_bound_var);
                #pragma omp parallel for reduction(+:roots_count)
                for (size_t i = 0; i < lb; ++i) {
                    double a = a_arr_r[i];
                    double b = b_arr_r[i];
                    double c = c_arr_r[i];
                    double D = b*b - 4.0*a*c;
                    if (D >= 0.0) {
                        double sd = sqrt(D);
                        double x1 = (-b + sd)/(2.0*a);
                        double x2 = (-b - sd)/(2.0*a);
                        (void)x1; (void)x2;
                        roots_count += 2;
                    }
                }
            }
        }
        else
        {
            if (mode == BASE) {
                size_t lb = (use_const_bound ? loop_bound_c : loop_bound_var);
                for (size_t i = 0; i < lb; ++i) {
                    double a = a_arr[i];
                    double b = b_arr[i];
                    double c = c_arr[i];
                    double D = b*b - 4.0*a*c;
                    if (D >= 0.0) {
                        double sd = sqrt(D);
                        double x1 = (-b + sd)/(2.0*a);
                        double x2 = (-b - sd)/(2.0*a);
                        (void)x1; (void)x2;
                        roots_count += 2;
                    }
                }
            }
            else if (mode == CONST_ALL) {
                size_t lb = (use_const_bound ? loop_bound_c : loop_bound_var);
                for (size_t i = 0; i < lb; ++i) {
                    double a = a_arr_c[i];
                    double b = b_arr_c[i];
                    double c = c_arr_c[i];
                    double D = b*b - 4.0*a*c;
                    if (D >= 0.0) {
                        double sd = sqrt(D);
                        double x1 = (-b + sd)/(2.0*a);
                        double x2 = (-b - sd)/(2.0*a);
                        (void)x1; (void)x2;
                        roots_count += 2;
                    }
                }
            }
            else if (mode == VOLATILE_ALL) {
                size_t lb = (use_const_bound ? loop_bound_c : loop_bound_var);
                for (size_t i = 0; i < lb; ++i) {
                    double a = a_arr_v[i];
                    double b = b_arr_v[i];
                    double c = c_arr_v[i];
                    double D = b*b - 4.0*a*c;
                    if (D >= 0.0) {
                        double sd = sqrt(D);
                        double x1 = (-b + sd)/(2.0*a);
                        double x2 = (-b - sd)/(2.0*a);
                        (void)x1; (void)x2;
                        roots_count += 2;
                    }
                }
            }
            else if (mode == RESTRICT_ALL) {
                size_t lb = (use_const_bound ? loop_bound_c : loop_bound_var);
                for (size_t i = 0; i < lb; ++i) {
                    double a = a_arr_r[i];
                    double b = b_arr_r[i];
                    double c = c_arr_r[i];
                    double D = b*b - 4.0*a*c;
                    if (D >= 0.0) {
                        double sd = sqrt(D);
                        double x1 = (-b + sd)/(2.0*a);
                        double x2 = (-b - sd)/(2.0*a);
                        (void)x1; (void)x2;
                        roots_count += 2;
                    }
                }
            }
        }

        auto t_end = clk::now();
        double elapsed = chrono::duration<double>(t_end - t_start).count();

        cout << "Variant: " << name
             << " | bound=" << (use_const_bound ? "const" : "var")
             << " | OpenMP=" << (with_openmp ? "ON" : "OFF")
             << " | Time=" << elapsed << " s"
             << " | Roots=" << roots_count
             << "\n";
    };

    vector<string> contracts = { "BASELINE", "CONST_ALL", "VOLATILE_ALL", "RESTRICT_ALL" };
    for (const auto &ct : contracts) {
        for (bool use_c : { false, true }) {     // false = var-bound, true = const-bound
            for (bool omp_on : { false, true }) { // false = без OpenMP, true = с OpenMP
                string name = ct + (omp_on ? "_OMP" : "_NOOMP");
                run_variant(name,
                            A.data(),
                            B.data(),
                            C.data(),
                            use_c,
                            omp_on);
            }
        }
    }
}


void experiment_useless_sum()
{
    cout << "\n=== Часть B: бесполезная сумма (volatile vs non-volatile) ===\n\n";

    static constexpr size_t M = 200'000'000;
    double *arr = new double[M];
    for (size_t i = 0; i < M; ++i) {
        arr[i] = 1.0;
    }

    {
        double sum = 0.0;
        auto t0 = clk::now();
        for (size_t i = 0; i < M; ++i) {
            sum += arr[i];
        }
        auto t1 = clk::now();
        double t = chrono::duration<double>(t1 - t0).count();
        cout << "Sum (non-volatile):\t time = " << t << " s"
             << " | sum_snapshot = " << sum << "\n";
    }

    {
        volatile double sum2 = 0.0;
        auto t0 = clk::now();
        for (size_t i = 0; i < M; ++i) {
            sum2 += arr[i];
        }
        auto t1 = clk::now();
        double t = chrono::duration<double>(t1 - t0).count();
        cout << "Sum (volatile):\t time = " << t << " s"
             << " | sum_snapshot = " << sum2 << "\n";
    }

    delete [] arr;
}


int main()
{
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    std::cout << "\nЛабораторная №9: контракты с компилятором (с OpenMP)\n";
    std::cout << "====================================================\n\n";

    experiment_quadratic();

    experiment_useless_sum();

    return 0;
}