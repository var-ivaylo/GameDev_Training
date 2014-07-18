#pragma once

#include <SDL.h>

#include <stdint.h>

class Timer
{
private:
	uint32_t mStartTicks;
	uint32_t mPausedTicks;

	bool mPaused;
	bool mStarted;
public:
	Timer();

	bool isStarted() { return this->mStarted; }

	bool isPaused() { return this->mStarted && this->mPaused; }

	uint32_t getTicks();

	void Start();

	void Stop();

	void Pause();

	void UnPause();
};