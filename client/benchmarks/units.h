#pragma once

#include <chrono>

namespace benchfactor
{
    using hclock_t = std::chrono::high_resolution_clock;

    constexpr int offset_scale(int n, int size) {
        return n * size;
    }

	inline namespace literals
	{
        constexpr unsigned long long operator""_GiB(const unsigned long long size)
        {
            return size * 1024L * 1024L * 1024L;
        }

        constexpr unsigned long long operator""_MiB(const unsigned long long size)
        {
            return size * 1024L * 1024L;
        }

        constexpr unsigned long long operator""_KiB(const unsigned long long size)
        {
            return size * 1024L;
        }

        constexpr unsigned long long operator""_GB(const unsigned long long size)
        {
            return size * 1000L * 1000L * 1000L;
        }

        constexpr unsigned long long operator""_MB(const unsigned long long size)
        {
            return size * 1000L * 1000L;
        }

        constexpr unsigned long long operator""_KB(const unsigned long long size)
        {
            return size * 1000L;
        }

        constexpr unsigned long long operator""_B(const unsigned long long size)
        {
            return size;
        }
	}
}
