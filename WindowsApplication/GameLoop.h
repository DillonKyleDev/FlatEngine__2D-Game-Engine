#pragma once
#include "SDL.h"
#include "Button.h"
//#include "Animation.h"
#include "GameScript.h"
#include "Mover.h"
#include "./scripts/Up.h"
#include "./scripts/Sinwave.h"
#include "./scripts/Attack.h"



namespace FlatEngine
{
	class GameLoop
	{
	public:
		GameLoop();
		~GameLoop();

		void Start();
		void Update();
		void Stop();
		void Pause();
		void Unpause();
		int TimeEllapsed();
		bool IsStarted();
		bool IsPaused();
		float GetAverageFps();
		float GetFramesCounted();
		void AddFrame();
		float GetDeltaTime();

	private:
		Uint32 startTime;
		int countedTicks;
		int pausedTicks;
		bool _started;
		bool _paused;
		float framesCounted;
		// For deltaTime
		float lastFrameTime;
		float deltaTime;

		std::vector<std::shared_ptr<GameObject>> gameObjects;
		std::vector<std::shared_ptr<GameScript>> scripts;
		std::shared_ptr<Mover> moverScript;
		std::shared_ptr<Up> upScript;
		std::shared_ptr<Sinwave> sinwaveScript;
		std::shared_ptr<Attack> attackScript;
	};
}

