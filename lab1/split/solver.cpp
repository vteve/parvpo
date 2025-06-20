#include <iostream>
#include <cmath>
#include <tuple>
#include <optional>
#include "solver.h"

double Discriminant(double a, double b, double c){
    return b*b-4*a*c;
}

double root1(double a, double b, double D){
    return (-b + sqrt(D))/(2*a);
}

double root2(double a, double b, double D){
    return (-b - sqrt(D))/(2*a);
}

std::optional<std::tuple<double, double>> solveQuadratic(double a, double b, double c){
    double D = Discriminant(a, b, c);
    if(D < 0) return std::nullopt;
    double x1 = root1(a, b, D);
    double x2 = root2(a, b, D);
    return (D == 0)? std::make_tuple(x1,x1) : std::make_tuple(x1, x2);
}
