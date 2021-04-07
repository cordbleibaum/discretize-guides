#include "ram.h"

#include <thread>
#include <vector>
#include <chrono>

#include "units.h"

using namespace benchfactor;
using namespace benchfactor::literals;

// The ram benchmark tests solely the bandwidth, it is inspired by McCalpin's STREAM benchmark,
// using a stream triad and multi-threading to utilize ram bandwidth
//
// We will later add a benchmark for latencies, and if more cache levels like Intel Optane become common we
// might need different sized tests

// Verification status: verified that it measures the right thing
// AVX2 is important for the compiler to optimize properly
// Outputs right numbers now

namespace benchfactor::benchmarks {
	template<int threads>
	void stream_triad(const float* __restrict A, const float* __restrict B, float* __restrict C, float scalar, size_t size) noexcept
	{
		#pragma omp parallel for schedule(static) num_threads(threads)
		for (int j = 0; j < size; j++)
		{
			C[j] = A[j] + scalar * B[j];
		}
	}

	template<int threads>
	double benchmark_bandwidth()
	{
		constexpr auto elementsPerArray = 4_GiB/sizeof(float)/3;

		auto* A = new float[elementsPerArray];
		auto* B = new float[elementsPerArray];
		auto* C = new float[elementsPerArray];
		
		#pragma omp parallel for schedule(static) num_threads(threads)
		for (int j = 0; j < elementsPerArray; ++j)
		{
			A[j] = 1.0f;
			B[j] = 1.0f;
			C[j] = 1.0f;
		}

		double lowestSeconds = DBL_MAX;

		for (int run = 0; run < 10; ++run) {
			const auto startTime = hclock_t::now();
			
			stream_triad<threads>(A, B, C, 1.0f, elementsPerArray);

			const auto duration = hclock_t::now() - startTime;
			const double currentSeconds = std::chrono::duration<double>(duration).count();

			lowestSeconds = lowestSeconds > currentSeconds ? currentSeconds : lowestSeconds;
		}

		delete[] A;
		delete[] B;
		delete[] C;

		// 3 Loads, 1 Store => GiB/s, assuming no streaming store for C
		const double bandwidth = 4.0 * elementsPerArray * sizeof(float) / lowestSeconds/1024.0/1024.0/1024.0;
		
		return bandwidth;
	}

	double ram_bandwidth_singlecore() 
	{
		return benchmark_bandwidth<1>();
	}

	// 4 Cores were chosen because most consumer CPU's only have 2 channel memory access, 
	// so this is a compromise between consumer and HEDT CPUs
	double ram_bandwidth_multicore() 
	{
		return benchmark_bandwidth<4>();
	}
}