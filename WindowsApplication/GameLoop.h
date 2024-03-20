#pragma once
#include "SDL.h"
#include "Button.h"
#include "GameScript.h"
<<<<<<< HEAD
#include "./scripts/BoardSquare.h"
#include "./scripts/GameManager.h"
#include "./scripts/GameManager.h"
#include "./scripts/GameBoard.h"
#include "./scripts/UndoButton.h"
#include "./scripts/StartButton.h"
#include "./scripts/RestartButton.h"
#include "./scripts/QuitButton.h"
#include "./scripts/AI.h"
=======
//#include "Mover.h"
#include "./scripts/Up.h"
//#include "./scripts/Sinwave.h"
//#include "./scripts/Attack.h"
//#include "./scripts/StartButton.h"
//#include "./scripts/MainMenuButton.h"
>>>>>>> 6974453e988b22781865c7731372dceb003c11ee



namespace FlatEngine
{
	class GameLoop
	{
	public:
		GameLoop();
		~GameLoop();

		void Start();
		void InitializeScriptObjects();
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
		std::shared_ptr<GameManager> gameManager;

		std::string startedScene;
		std::vector<std::shared_ptr<GameObject>> gameObjects;
		std::vector<std::shared_ptr<GameScript>> scripts;
		std::vector<std::shared_ptr<GameScript>> activeScripts;
<<<<<<< HEAD
=======
		//std::shared_ptr<Up> upScript;
>>>>>>> 6974453e988b22781865c7731372dceb003c11ee
	};
}

