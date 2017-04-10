#pragma once

#include <SDL.h>

struct TimerManager
{
	struct Timer
	{
		Uint32 startTicks;
		Uint32 pausedTicks;
		Uint8 frameCap;
		bool isPaused;
		bool isStarted;
		bool isFrameCap = false;
	} timer;

	void startTimer()
	{
		timer.isStarted = true;
		timer.isPaused = false;

		timer.startTicks = SDL_GetTicks();
		timer.pausedTicks = 0;
	}

	bool isTimerStarted() const
	{
		return timer.isStarted;
	}

	bool isTimerPaused() const
	{
		return timer.isPaused;
	}

	bool isTimerStoped() const
	{
		return timer.isPaused;
	}

	void stopTimer()
	{
		timer.isStarted = false;
		timer.isPaused = false;

		timer.startTicks = 0;
		timer.pausedTicks = 0;
	}

	void pauseTimer()
	{
		timer.isStarted = false;
		timer.isPaused = true;

		timer.pausedTicks = SDL_GetTicks() - timer.startTicks;
		timer.startTicks = 0;
	}

	void unPauseTimer()
	{
		timer.isStarted = true;
		timer.isPaused = false;

		timer.startTicks = SDL_GetTicks() - timer.pausedTicks;
		timer.pausedTicks = 0;
	}

	Uint32 getTicks() const
	{
		Uint32 time = 0;

		if (timer.isStarted)
			if (timer.isPaused)
				time = timer.pausedTicks;
			else
				time = SDL_GetTicks() - timer.startTicks;

		return time;
	}

	void setFrameCap(bool cap)
	{
		timer.isFrameCap = cap;
	}

	bool isFrameCap() const
	{
		return timer.isFrameCap;
	}
};
