#include <cstdlib>
#include <map>
#include <functional>
#include <string>
#include <iostream>

#include <nlohmann/json.hpp>

#include "../benchmarks/ram.h"

using json = nlohmann::json;

int main() {
    std::map<std::string, std::function<double()>> benchmarks;
    json results;

    benchmarks["ram_bandwidth_multicore"] = benchfactor::benchmarks::ram_bandwidth_multicore;

    for (const auto& [name, function] : benchmarks) {
        results[name] = function();
    }

    std::cout << results.dump(4);

    return EXIT_SUCCESS;
}
