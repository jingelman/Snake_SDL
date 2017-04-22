#pragma once

#include <SDL.h>

namespace TimerManager
{
	struct Timer
	{
		uint32_t startTicks;
		uint32_t pausedTicks;
		bool isPaused;
		bool isStarted;
	} static timer;;

	static void startTimer()
	{
		timer.isStarted = true;
		timer.isPaused = false;

		timer.startTicks = SDL_GetTicks();
		timer.pausedTicks = 0;
	}

	static bool isTimerStarted()
	{
		return timer.isStarted;
	}

	static bool isTimerPaused()
	{
		return timer.isPaused;
	}

	static bool isTimerStoped()
	{
		return !timer.isPaused && !timer.isStarted;
	}

	static void stopTimer()
	{
		timer.isStarted = false;
		timer.isPaused = false;

		timer.startTicks = 0;
		timer.pausedTicks = 0;
	}

	static void pauseTimer()
	{
		timer.isStarted = false;
		timer.isPaused = true;

		timer.pausedTicks = SDL_GetTicks() - timer.startTicks;
		timer.startTicks = 0;
	}

	static void unPauseTimer()
	{
		timer.isStarted = true;
		timer.isPaused = false;

		timer.startTicks = SDL_GetTicks() - timer.pausedTicks;
		timer.pausedTicks = 0;
	}

	static uint32_t getTicks()
	{
		uint32_t time = 0;

		if (timer.isStarted)
			if (timer.isPaused)
				time = timer.pausedTicks;
			else
				time = SDL_GetTicks() - timer.startTicks;

		return time;
	}
};
