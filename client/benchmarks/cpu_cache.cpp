#include "cpu.h"

#include <cstdlib>
#include <immintrin.h>

#include "measurement.h"
#include "units.h"

using namespace benchfactor;
using namespace benchfactor::literals;

namespace benchfactor::benchmarks {
	// This tests cache bandwidths by summing up a vector of different sizes to hit the different caches

	// Verification status: verified that it measures the right thing

	// This adds up a vector using AVX
	float vec_add(float* data, int size)
	{
		float sum = 0;

		__m256 sum1 = _mm256_setzero_ps();
		__m256 sum2 = _mm256_setzero_ps();
		__m256 sum3 = _mm256_setzero_ps();
		__m256 sum4 = _mm256_setzero_ps();

		int i;

		for (i = 0; i < size - 31; i += 32)
		{
			const __m256 vec1 = _mm256_loadu_ps(data + i);
			const __m256 vec2 = _mm256_loadu_ps(data + i + offset_scale(8, sizeof(float)));
			const __m256 vec3 = _mm256_loadu_ps(data + i + offset_scale(16, sizeof(float)));
			const __m256 vec4 = _mm256_loadu_ps(data + i + offset_scale(24, sizeof(float)));

			sum1 = _mm256_add_ps(sum1, vec1);
			sum2 = _mm256_add_ps(sum2, vec2);
			sum3 = _mm256_add_ps(sum3, vec3);
			sum4 = _mm256_add_ps(sum4, vec4);
		}

		float v1[8];
		float v2[8];
		float v3[8];
		float v4[8];
		_mm256_storeu_ps(v1, sum1);
		_mm256_storeu_ps(v2, sum2);
		_mm256_storeu_ps(v3, sum3);
		_mm256_storeu_ps(v4, sum4);

		for (int j = 0; j < 8; j++)
		{
			sum += v1[j];
			sum += v2[j];
			sum += v3[j];
			sum += v4[j];
		}

		for (; i < size; ++i)
		{
			sum += data[i];
		}

		return sum;
	}

	// This tests caches by accessing different sized arrays using an optimized vec_add function
	// It is expected to have relatively similar access times, so we expect a bandwidth benchmark to be sufficient
	// A template is used to avoid large caches from dominating runtime and separating benchmarks
	template<int size>
	inline double cpu_cache()
	{
		constexpr size_t dataSize = size / sizeof(float);
		auto* data = new float[dataSize];

		for (int i = 0; i < dataSize; ++i)
		{
			data[i] = 1.0f;
		}

		double iterationsPerSecond = measureIterationsPerSecond(5.0, [&](int iterations) {
			for (int i = 0; i < iterations; ++i)
			{
				volatile float result = vec_add(data, size / sizeof(float));
			}
			});

		delete[] data;

		return iterationsPerSecond;
	}

	// This should definitely hit L1
	double cpu_cache_small() {
		return cpu_cache<2_KiB>();
	}

	// This may hit L1 or L2
	double cpu_cache_mid() {
		return cpu_cache<128_KiB>();
	}

	// This may hit L2 or L3
	double cpu_cache_large() {
		return cpu_cache<1_MiB>();
	}

	// This should definitely hit L3
	double cpu_cache_huge() {
		return cpu_cache<8_MiB>();
	}
}