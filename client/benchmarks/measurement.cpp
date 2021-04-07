#pragma once

#include "measurement.h"

#include <chrono>
#include <cmath>

namespace benchfactor
{
	// Benchmark procedure reducing impact of background noise
	// arXiv:1608.04295 [cs.PF]
	double measureIterationsPerSecond(double timeBudget, const std::function<void(int)>& benchmarkFunction) {
		// Assumptions for timing accuracy and precision, improve later
		double t_acc = (double) std::chrono::high_resolution_clock::period::num / std::chrono::high_resolution_clock::period::den; // will be usually around 1ns
		double t_prec = 0.000001; // 1000ns, assumption from guessing, too large estimations don't pose much harm

		// Pre-Measurement for needed number of runs
		int j = t_acc / t_prec;
		double t_p0 = INFINITY;

		for (int i = 1; i <= j; i++) {
			const auto start = std::chrono::high_resolution_clock::now();
			benchmarkFunction(i);
			const auto duration = std::chrono::high_resolution_clock::now() - start;
			t_p0 = std::fmin(t_p0, std::chrono::duration<double>(duration).count()/i);
		}

		// Oracle function suggestion from paper, should likely be replaced by LUT in a later version
		double a = 0.009/t_prec;
		double b = 0.5;

		int iterations = (int) std::floor(1.0 + (j - 1)/(1.0 + std::exp(a*(t_p0-b*t_acc))));

		// Actual measurement
		const auto startTotal = std::chrono::high_resolution_clock::now();
		double bestIpS = 0;

		do {
			const auto startTime = std::chrono::high_resolution_clock::now();
			benchmarkFunction(iterations);
			const auto duration = std::chrono::high_resolution_clock::now() - startTime;
			double runtime = std::chrono::duration<double>(duration).count();

			bestIpS = std::fmax(iterations / runtime, bestIpS);
		} while (std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - startTotal).count() < timeBudget);

		return bestIpS;
	}
}