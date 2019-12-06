#pragma once

#include <SDL.h>

class Timer
{
public:
	inline void Start()
	{
		frameStart = SDL_GetTicks();
		deltaTime = (float)(frameStart - lastFrame);
		lastFrame = frameStart;
	}

	inline void End() 
	{
		frameTime = SDL_GetTicks() - frameStart;
		if (frameDelay > frameTime)
			SDL_Delay(frameDelay - frameTime);
	}

	inline float GetDeltaTime()
	{
		return deltaTime;
	}

private:
	int maxFPS = 60;
	int frameDelay = 1000 / maxFPS;

	int frameStart = 0;
	int frameTime = 0;
	int lastFrame = 0;
	float deltaTime = 0;
};

extern Timer g_Timer;