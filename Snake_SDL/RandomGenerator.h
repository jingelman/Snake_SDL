#pragma once

#include <random>

inline Uint8 randomNumber(const Uint8 min, const Uint8 max)
{
	std::uniform_int_distribution<int> distValue(min, max);
	std::random_device rd;
	std::mt19937 generator(rd());

	return distValue(generator);
}
