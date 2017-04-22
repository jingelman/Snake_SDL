#pragma once

#include <random>
#include <stdio.h>

inline uint8_t randomNumber(const uint8_t min, const uint8_t max)
{
	std::uniform_int_distribution<int> distValue(min, max);
	std::random_device rd;
	std::mt19937 generator(rd());

	return distValue(generator);
}