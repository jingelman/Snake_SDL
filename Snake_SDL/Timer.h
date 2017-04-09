#pragma once

#include <SDL.h>

struct Timer
{
	Uint32 startTicks;
	Uint32 pausedTicks;
	Uint8 frameCap;
	bool isPaused;
	bool isStarted;
	bool isFrameCap = false;
};

inline void startTimer(Timer &t)
{
	t.isStarted = true;
	t.isPaused = false;

	t.startTicks = SDL_GetTicks();
	t.pausedTicks = 0;

}

inline bool isTimerStarted(Timer &t)
{
	return t.isStarted;
}

inline bool isTimerPaused(Timer &t)
{
	return t.isPaused;
}

inline bool isTimerStoped(Timer &t)
{
	return t.isPaused;
}


inline void stopTimer(Timer &t)
{
	t.isStarted = false;
	t.isPaused = false;

	t.startTicks = 0;
	t.pausedTicks = 0;
}

inline void pauseTimer(Timer &t)
{
	t.isStarted = false;
	t.isPaused = true;

	t.pausedTicks = SDL_GetTicks() - t.startTicks;
	t.startTicks = 0;
}

inline void unPauseTimer(Timer &t)
{
	t.isStarted = true;
	t.isPaused = false;

	t.startTicks = SDL_GetTicks() - t.pausedTicks;
	t.pausedTicks = 0;	
}

inline Uint32 getTicks(Timer &t)
{
	Uint32 time = 0;

	if (t.isStarted)
		if (t.isPaused)
			time = t.pausedTicks;
		else
			time = SDL_GetTicks() - t.startTicks;
		
	return time;
}

inline void setFrameCap(Timer &t, bool cap)
{
	t.isFrameCap = cap;
}

inline bool isFrameCap(Timer &t)
{
	return t.isFrameCap;
}