#pragma once
#include "SDL.h"
#include "Button.h"
#include "GameScript.h"
#include "./scripts/Up.h"
#include "./scripts/GameBoard.h"
#include "./scripts/King.h"
#include "./scripts/Queen.h"
#include "./scripts/Bishop.h"
#include "./scripts/Knight.h"
#include "./scripts/Rook.h"
#include "./scripts/Pawn.h"



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

		std::string startedScene;
		std::vector<std::shared_ptr<GameObject>> gameObjects;
		std::vector<std::shared_ptr<GameScript>> scripts;
		std::shared_ptr<Up> upScript;
		std::shared_ptr<GameBoard> gameBoardScript;
		std::shared_ptr<King> kingScript;
		std::shared_ptr<Queen> queenScript;
		std::shared_ptr<Bishop> bishopScript;
		std::shared_ptr<Knight> knightScript;
		std::shared_ptr<Rook> rookScript;
		std::shared_ptr<Pawn> pawnScript;

	};
}

