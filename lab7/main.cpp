#include <iostream>
#include <vector>
#include <cmath>
#include <random>
#include <chrono>
#include <fstream>

#define NUM 50'000'000
#define SEED 42

using namespace std;
using namespace chrono;

struct EquationResult {
    double a, b, c;
    int num_roots;
    double root1, root2;
};

int solve_quadratic(double a, double b, double c, double &x1, double &x2) {
    double d = b * b - 4 * a * c;
    if (d > 0) {
        double sqrt_d = sqrt(d);
        x1 = (-b + sqrt_d) / (2 * a);
        x2 = (-b - sqrt_d) / (2 * a);
        return 2;
    } else if (d == 0) {
        x1 = x2 = -b / (2 * a);
        return 1;
    } else {
        x1 = x2 = 0;
        return 0;
    }
}

int main() {
    mt19937 rng(SEED);
    uniform_real_distribution<double> dist(-1000.0, 1000.0);
    vector<EquationResult> results;
    results.reserve(NUM);
    auto start = high_resolution_clock::now();
    for (size_t i = 0; i < NUM; ++i) {
        double a = dist(rng);
        while (fabs(a) < 1e-6) a = dist(rng);
        double b = dist(rng);
        double c = dist(rng);
        double x1, x2;
        int num_roots = solve_quadratic(a, b, c, x1, x2);
        results.push_back({a, b, c, num_roots, x1, x2});
    }
    auto end = high_resolution_clock::now();
    double elapsed = duration<double>(end - start).count();
    cout << "Solved " << NUM << " equations in " << elapsed << " seconds." << endl;
    return 0;
}