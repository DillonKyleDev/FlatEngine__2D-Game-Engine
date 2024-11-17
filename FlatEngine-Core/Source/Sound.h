#pragma once
#include <string>
#include <SDL_mixer.h>


namespace FlatEngine
{
	class Sound
	{
	public:
		Sound();
		~Sound();

		Mix_Music* LoadMusic(std::string path);
		Mix_Chunk* LoadEffect(std::string path);
		void PlayMusic();
		void PauseMusic();
		bool IsPaused();
		void StopMusic();
		bool IsMusicPlaying();
		void PlayEffect(int channel = -1);
		void HaultChannel(int channel = -1);

	private:
		Mix_Music* m_music;
		Mix_Chunk* m_effect;
		std::string m_path;
		bool m_b_isPaused;
	};
}
