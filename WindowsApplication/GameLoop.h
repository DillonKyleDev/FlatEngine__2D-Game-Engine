#pragma once
#include "SDL.h"
#include "GameScript.h"
#include "Mover.h"
#include "./scripts/Up.h"
#include "./scripts/Sinwave.h"


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
		// Both for deltaTime
		float lastFrameTime;
		float deltaTime;

		std::vector<GameScript*> scripts;

		Mover* moverScript;
		Up* upScript;
		Sinwave* sinwaveScript;
	};
}

