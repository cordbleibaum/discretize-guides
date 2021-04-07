#include <thread>
#include <cstdlib>
#include <immintrin.h>
#include <omp.h>

#include "cpu.h"
#include "measurement.h"
#include "units.h"

using namespace benchfactor;
using namespace benchfactor::literals;

namespace benchfactor::benchmarks {
	template<int threads>
	void init_jacobi(double* grid, int width, int height) noexcept
	{
		// Init needs to be done multi-thread to potentially pay attention to first touch policy in case of NUMA
		#pragma omp parallel for schedule(static) num_threads(threads)
		for (int y = 1; y < height - 1; ++y) {
			for (int x = 0; x < width; ++x) {
				grid[y * width + x] = 0.0;
			}
		}

		for (int i = 0; i < width; ++i) {
			grid[i] = 1.0;
		}

		for (int i = 0; i < height; ++i) {
			grid[i * width] = 1.0;
		}
	}

	// Generic jacobi relaxation solver using AVX and multiple cores
	//
	// Verification status: verified that it is working as intended, removed blocking to ensure that there are no too different caching effects
	template<int threads>
	double* jacobi(double* grid1, double* grid2, int width, int height, const int timesteps) noexcept
	{
		double* grid_new = grid1;
		double* grid_old = grid2;

		const __m256d div4 = _mm256_set1_pd(1.0 / 4.0);

		for (int t = 0; t < timesteps; ++t) {
			#pragma omp parallel for schedule(static) num_threads(threads)
			for (int y = 1; y < height - 1; ++y) {
				for (int x = 1; x < width - 1; ++x) {
					const __m256d xMinus1 = _mm256_loadu_pd(grid_old + y * width + (x - 1));
					const __m256d yPlus1 = _mm256_loadu_pd(grid_old + (y + 1) * width + x);
					const __m256d yMinus1 = _mm256_loadu_pd(grid_old + (y - 1) * width + x);
					__m256d newValues = _mm256_loadu_pd(grid_old + y * width + (x + 1));
					newValues = _mm256_add_pd(newValues, xMinus1);
					newValues = _mm256_add_pd(newValues, yPlus1);
					newValues = _mm256_add_pd(newValues, yMinus1);

					newValues = _mm256_mul_pd(newValues, div4);

					_mm256_storeu_pd(grid_new + y * width + x, newValues);
				}
			}
			double* tmp = grid_new;
			grid_new = grid_old;
			grid_old = tmp;
		}

		return grid_old;
	}

	// This tests a multi-core high load scenario by doing jacobi iterations using AVX
	// Make sure this is compiled with OpenMP!
	//
	// Verification status: verified that it is working as intended
	template<int threads, int size>
	double benchmark_simd()
	{
		constexpr size_t bytes = size;

		const size_t width = sqrt(bytes / (2L * sizeof(double)));
		const size_t height = sqrt(bytes / (2L * sizeof(double)));

		double runtime = 0;
		int timesteps;

		// Longer min-runtime is chosen to potentially observe effects of insufficient cooling
		constexpr double minRuntime = 5.0;

		for (timesteps = 1; runtime < minRuntime; timesteps *= 2) {
			auto* grid1 = new double[width * height];
			auto* grid2 = new double[width * height];

			init_jacobi<threads>(grid1, width, height);
			init_jacobi<threads>(grid2, width, height);

			const auto startTime = hclock_t::now();

			volatile double* result = jacobi<threads>(grid1, grid2, width, height, timesteps);

			const auto duration = hclock_t::now() - startTime;
			runtime = std::chrono::duration<double>(duration).count();

			delete[] grid1;
			delete[] grid2;
		}
		timesteps /= 2;

		const double lups = timesteps * (width - 2) * (height - 2) / runtime / 1000.0 / 1000.0;

		return lups;
	}

	// Sizes are chosen to be in cache to avoid bottlenecks that would need to be tuned to the machine
	// Different sizes are adjusted to get different caching
	double cpu_calculation_sequential()
	{
		return benchmark_simd<1, 8_MiB>();
	}

	// 4 Cores is chosen as a middle ground for older games and applications
	double cpu_calculation_multicore()
	{
		return benchmark_simd<4, 8_MiB>();
	}

	// 16 Cores is chosen to have a realistic middleground for current desktop CPU's
	double cpu_calculation_manycore()
	{
		return benchmark_simd<16, 16_MiB>();
	}
}
