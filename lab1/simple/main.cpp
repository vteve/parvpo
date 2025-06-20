#include <iostream>
#include <cmath>
#include <random>
#include <chrono>
#include <tuple>
#include <optional>

#define NUM 100000000
#define SEED 42

inline double Discriminant(double a, double b, double c){
    return b*b-4*a*c;
}

inline double root1(double a, double b, double D){
    return (-b + sqrt(D))/(2*a);
}

inline double root2(double a, double b, double D){
    return (-b - sqrt(D))/(2*a);
}

inline std::optional<std::tuple<double, double>> solveQuadratic(double a, double b, double c){
    double D = Discriminant(a, b, c);
    if(D < 0) return std::nullopt;
    double x1 = root1(a, b, D);
    double x2 = root2(a, b, D);
    return (D == 0)? std::make_tuple(x1,x1) : std::make_tuple(x1, x2);
}


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