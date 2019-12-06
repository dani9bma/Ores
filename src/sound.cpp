#include "sound.h"

#include <stdio.h>

Sound::Sound(const char* path)
{
	m_Sound = Mix_LoadMUS(path);
	if (!m_Sound)
	{
		printf("Could not load sound: %s\n", SDL_GetError());
		exit(1);
	}
}

void Sound::Destroy()
{
	Mix_FreeMusic(m_Sound);
}

void Sound::Play(bool loop)
{
	if (Mix_PlayMusic(m_Sound, loop ? -1 : 0) == -1)
	{
		printf("Could not play sound: %s\n", SDL_GetError());
		exit(1);
	}
}

void Sound::Pause()
{
	Mix_PauseMusic();
}
