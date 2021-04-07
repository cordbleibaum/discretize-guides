#pragma once

#include <functional>
#include <chrono>

namespace benchfactor
{
    double measureIterationsPerSecond(double timeBudget, const std::function<void(int)> &benchmarkFunction);
}
