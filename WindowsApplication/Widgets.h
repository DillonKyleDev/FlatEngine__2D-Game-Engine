#pragma once
#include <vector>
#include "Canvas.h"
#include "Button.h"



namespace FlatEngine { namespace FlatGui
{
	class Widgets
	{
	public:
		Widgets();
		~Widgets();

		// Scene Canvas Management
		void AddSceneCanvas(Canvas* canvas);
		void RemoveSceneCanvas(Canvas* canvas);
		std::vector<Canvas*> GetSceneCanvases();

		void AddHoveredSceneButton(Button* button);
		void RemoveHoveredSceneButton(Button* button);

		void IncrementSceneCanvasID();
		long GetNextSceneCanvasID();

		Button* GetTopLevelSceneButton();
		std::vector<Button*> GetHoveredSceneButtons();

		// Game Canvas Management
		void AddHoveredGameButton(Button* button);
		void RemoveHoveredGameButton(Button* button);
		void IncrementGameCanvasID();
		long GetNextGameCanvasID();
		Button* GetTopLevelGameButton();
		std::vector<Button*> GetHoveredGameButtons();

	private:
		long nextSceneCanvasID;
		long nextGameCanvasID;
		std::vector<Button*> sceneHoveredButtons;
		std::vector<Button*> gameHoveredButtons;
		std::vector<Canvas*> sceneCanvases;
		std::vector<Canvas*> gameCanvases;
	};
}
}
