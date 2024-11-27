#include "Sound.h"
#include "FlatEngine.h"


namespace FlatEngine
{
	Sound::Sound()
	{
		m_path = "";
		m_b_isPaused = false;
		m_music = nullptr;
		m_effect = nullptr;
		m_musicVolume = 20;
		m_effectVolume = 7;
		Mix_Volume(-1, m_effectVolume);
		Mix_VolumeMusic(m_musicVolume);
	}
	Sound::~Sound()
	{
		Mix_FreeMusic(m_music);
		Mix_FreeChunk(m_effect);
		m_music = nullptr;
		m_effect = nullptr;
	}

	int Sound::getMusicVolume()
	{
		return Mix_VolumeMusic(-1);
	}

	void Sound::setMusicVolume(int volume)
	{
		Mix_VolumeMusic(volume);
	}

	int Sound::getEffectVolume()
	{
		return Mix_Volume(-1, -1);
	}

	void Sound::setEffectVolume(int volume)
	{
		Mix_Volume(-1, volume);
	}

	Mix_Music* Sound::LoadMusic(std::string path)
	{
		m_music = Mix_LoadMUS(path.c_str());
		return m_music;
	}

	Mix_Chunk* Sound::LoadEffect(std::string path)
	{
		m_effect = Mix_LoadWAV(path.c_str());
		return m_effect;
	}

	void Sound::PlayMusic()
	{
		if (Mix_PlayingMusic() == 0)
		{
			Mix_PlayMusic(m_music, -1);
		}
		else
		{
			if (Mix_PausedMusic() == 1)
			{
				Mix_ResumeMusic();
			}
			else
			{
				Mix_PauseMusic();
			}
		}
	}

	void Sound::PauseMusic()
	{
		if (Mix_PausedMusic() == 1)
		{
			Mix_ResumeMusic();
		}
		else
		{
			Mix_PauseMusic();
		}
	}

	bool Sound::IsPaused()
	{
		return Mix_PausedMusic() == 1;
	}

	void Sound::StopMusic()
	{
		Mix_HaltMusic();
	}

	bool Sound::IsMusicPlaying()
	{
		return Mix_PlayingMusic();
	}

	void Sound::PlayEffect(int channel)
	{
		// Play effect on channel, repeat 0 times
		Mix_PlayChannel(channel, m_effect, 0);
	}

	void Sound::HaultChannel(int channel)
	{
		Mix_HaltChannel(channel);
	}
}