#pragma once
#include "FlatEngine.h"
#include "GameLoop.h"
#include <string>

namespace FlatEngine
{
	class Application
	{
	public:
		Application()
		{
			_hasQuit = false;
		}
		~Application() {};

		virtual void OnInit() {};
		virtual void Run() {};
		virtual FlatEngine::GameLoop* GetGameLoop() { return nullptr; };
		virtual bool GameLoopStarted() { return false; };
		virtual bool GameLoopPaused() { return false; };
		virtual void StartGameLoop() {};
		virtual void UpdateGameLoop() {};
		virtual void PauseGameLoop() {};
		virtual void StopGameLoop() {};
		virtual void PauseGame() {};

		void Quit() { _hasQuit = true; };
		bool& HasQuit() { return _hasQuit; };
		void OnLoadScene(std::string sceneName) {};
		
	private:
		bool _hasQuit;
	};
}