#pragma once

namespace benchfactor::benchmarks {
    double cpu_cache_small();
    double cpu_cache_mid();
    double cpu_cache_large();
    double cpu_cache_huge();

    double cpu_calculation_sequential();
    double cpu_calculation_multicore();
    double cpu_calculation_manycore();
}
