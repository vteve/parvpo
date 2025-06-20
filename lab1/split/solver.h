#ifndef SOLVER_H
#define SOLVER_H

#include <tuple>
#include <optional>

std::optional<std::tuple<double, double>> solveQuadratic(double a, double b, double c);

#endif