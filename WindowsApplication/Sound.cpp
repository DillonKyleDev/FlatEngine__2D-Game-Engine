#include "Sound.h"
#include "FlatEngine.h"


namespace FlatEngine
{
	Sound::Sound()
	{
		path = "";
		_isPaused = false;
		music = nullptr;
		effect = nullptr;
	}
	Sound::~Sound()
	{
		music = nullptr;
		effect = nullptr;
	}

	Mix_Music* Sound::LoadMusic(std::string path)
	{
		this->music = Mix_LoadMUS(path.c_str());
		return this->music;
	}

	Mix_Chunk* Sound::LoadEffect(std::string path)
	{
		this->effect = Mix_LoadWAV(path.c_str());
		return this->effect;
	}

	void Sound::PlayMusic()
	{
		if (Mix_PlayingMusic() == 0)
		{
			//Play the music
			Mix_PlayMusic(this->music, -1);
		}
		//If music is being played
		else
		{
			//If the music is paused
			if (Mix_PausedMusic() == 1)
			{
				//Resume the music
				Mix_ResumeMusic();
			}
			//If the music is playing
			else
			{
				//Pause the music
				Mix_PauseMusic();
			}
		}
	}

	void Sound::PauseMusic()
	{
		//If the music is paused
		if (Mix_PausedMusic() == 1)
		{
			//Resume the music
			Mix_ResumeMusic();
		}
		//If the music is playing
		else
		{
			//Pause the music
			Mix_PauseMusic();
		}
	}

	void Sound::StopMusic()
	{
		//Stop the music
		Mix_HaltMusic();
	}

	bool Sound::IsMusicPlaying()
	{
		return Mix_PlayingMusic();
	}

	void Sound::PlayEffect(int channel)
	{
		// Play effect on channel, repeat 0 times
		Mix_PlayChannel(channel, this->effect, 0);
	}

	void Sound::HaultChannel(int channel)
	{
		Mix_HaltChannel(channel);
	}
}