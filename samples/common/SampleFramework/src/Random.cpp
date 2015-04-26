#include "SampleFramework/Random.h"
#include <random>
#include <ctime>

std::mt19937 generator;

void SampleFramework::Random::initialize() {
	generator = std::mt19937(static_cast<unsigned>(time(nullptr)));
}

float SampleFramework::Random::randomFloat(const float min, const float max) {
	std::uniform_real_distribution<double> distribution(min, max);
	const double res = distribution(generator);
	return static_cast<float>(res);
}