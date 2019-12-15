#include "sound.h"

#include <stdio.h>

Music::Music(const char* path)
{
	m_Sound = Mix_LoadMUS(path);
	if (!m_Sound)
	{
		printf("Could not load sound: %s\n", SDL_GetError());
		exit(1);
	}
}

void Music::Destroy()
{
	Mix_FreeMusic(m_Sound);
}

void Music::Play(bool loop)
{
	if (Mix_PlayMusic(m_Sound, loop ? -1 : 0) == -1)
	{
		printf("Could not play sound: %s\n", SDL_GetError());
		exit(1);
	}
}

void Music::Pause()
{
	Mix_PauseMusic();
}

SoundEffect::SoundEffect(const char* path)
{
	m_Sound = Mix_LoadWAV(path);
	if (!m_Sound)
	{
		printf("Could not load sound: %s\n", SDL_GetError());
		exit(1);
	}
}

void SoundEffect::Destroy()
{
	Mix_FreeChunk(m_Sound);
}

void SoundEffect::Play()
{
	if (Mix_PlayChannel(-1, m_Sound, 0))
	{
		printf("Could not play sound: %s\n", SDL_GetError());
		exit(1);
	}
}
