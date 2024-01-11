#pragma once
#include "SDL.h"
#include "Button.h"
//#include "Animation.h"
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

		// For deltaTime
		float lastFrameTime;
		float deltaTime;

		std::vector<std::shared_ptr<GameObject>> gameObjects;
		std::vector<std::shared_ptr<GameScript>> scripts;
		
		// Variables holding GameObjects with specific Component types that need specific things done
		// ie. vector of all Buttons for button event handling and a vector of all Animation components in the Update() function.
		// These can be fetched once in GameLoop::Start() and then we only have to loop through exactly the GameObjects we know
		// we need for a given task during the GameLoop::Update() loop.

		std::vector<std::shared_ptr<Button>> buttonComponents;
		//std::vector<Animation*> animationComponents;

		std::shared_ptr<Mover> moverScript;
		std::shared_ptr<Up> upScript;
		std::shared_ptr<Sinwave> sinwaveScript;
	};
}

