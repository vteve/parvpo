#include <iostream>
#include <random>
#include <chrono>
#include "solver.h"

#define NUM 100000000
#define SEED 42


int main() {
    std::mt19937 rng(SEED);
    std::uniform_real_distribution<double> dist(-100.0, 100.0);
    int count = 0;
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < NUM; ++i) {
        double a = dist(rng);
        double b = dist(rng);
        double c = dist(rng);
        auto roots = solveQuadratic(a, b, c);
        if (roots) count++;
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "Time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " ms\n";
    std::cout << "Equations with real roots: " << count << std::endl;
    return 0;
}