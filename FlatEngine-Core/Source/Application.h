#pragma once
#include <string>
#include "GameLoop.h"

namespace FlatEngine
{
	class GameLoop;

	class Application
	{
	public:
		Application()
		{
			_hasQuit = false;
			m_windowWidth = 1920;
			m_windowHeight = 1080;
		}
		~Application() {};

		virtual void Init() {};
		virtual void Run() {};
		void SetWindowDimensions(int width, int height) { m_windowWidth = width; m_windowHeight = height; };
		int WindowWidth() { return m_windowWidth; };
		int WindowHeight() { return m_windowHeight; };
		void BeginRender(); // Defined in Application.cpp
		void EndRender();   // Defined in Application.cpp
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
		int m_windowWidth;
		int m_windowHeight;
	};
}