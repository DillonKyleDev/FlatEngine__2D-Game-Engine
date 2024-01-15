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
		void StopMusic();
		bool IsMusicPlaying();
		void PlayEffect(int channel = -1);
		void HaultChannel(int channel = -1);

		Mix_Music* music;
		Mix_Chunk* effect;

	private:
		std::string path;
		bool _isPaused;
	};
}
