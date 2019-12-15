#pragma once

#include <SDL_mixer.h>

class Music
{
public:
	Music(const char* path);

	void Destroy();

	void Play(bool loop);
	void Pause();

private:
	Mix_Music* m_Sound;
};

class SoundEffect
{
public:
	SoundEffect(const char* path);

	void Destroy();

	void Play();
private:
	Mix_Chunk* m_Sound;
};