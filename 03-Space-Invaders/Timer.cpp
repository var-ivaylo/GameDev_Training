#include "Timer.h"
#include <iostream>

Timer::Timer()
{
	this->mPausedTicks = 0;
	this->mStartTicks = 0;

	this->mPaused = false;
	this->mStarted = false;
}

uint32_t Timer::getTicks()
{
	if (this->isStarted())
	{
		if (this->mPaused)
		{
			return this->mPausedTicks;
		}
		else
		{
			return SDL_GetTicks() - this->mStartTicks;
		}
	}

	return 0;
}

void Timer::Start()
{
	this->mStarted = true;
	this->mPaused = false;

	this->mStartTicks = SDL_GetTicks();
	this->mPausedTicks = 0;
}

void Timer::Stop()
{
	this->mStarted = false;
	this->mPaused = false;

	this->mStartTicks = 0;
	this->mPausedTicks = 0;
}

void Timer::Pause()
{
	if (this->isStarted() && !this->isPaused())
	{
		this->mPaused = true;

		this->mPausedTicks = SDL_GetTicks() - this->mStartTicks;
		this->mStartTicks = 0;
	}
}

void Timer::UnPause()
{
	if (this->isStarted() && this->isPaused())
	{
		this->mPaused = false;

		this->mStartTicks = SDL_GetTicks() - this->mPausedTicks;
		this->mPausedTicks = 0;
	}
}