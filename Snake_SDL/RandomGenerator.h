#pragma once

#include <random>
#include <stdio.h>

inline Uint8 randomNumber(const Uint8 min, const Uint8 max)
{
	std::uniform_int_distribution<int> distValue(min, max);
	std::random_device rd;
	std::mt19937 generator(rd());

	return distValue(generator);
}

/* Dont work
inline char* concatStrings(Uint8 counter)
{
	char buf[20], snum[10];
	sprintf_s(snum, "%i", counter);
	snprintf(buf, sizeof buf, "%s%s%s", "You ate ", snum, " apples!");

	return buf;
}
*/