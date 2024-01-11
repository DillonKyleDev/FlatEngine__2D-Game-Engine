#pragma once
#include <vector>
#include "Canvas.h"
#include "Button.h"



namespace FlatEngine { namespace FlatGui
{
	class WidgetsManager
	{
	public:
		WidgetsManager();
		~WidgetsManager();

		// Scene Canvases
		std::shared_ptr<Canvas> Scene_CreateCanvas(long ID, long parentID, int layerNumber);
		void Scene_DeleteCanvas(long ID);
		std::vector<std::shared_ptr<Canvas>> Scene_GetCanvases();
		void Scene_IncrementCanvasID();
		long Scene_GetNextCanvasID();

		// Scene Buttons
		std::shared_ptr<Button> Scene_CreateButton(long ID, long parentID, int layerNumber);
		void Scene_DeleteButton(long ID);
		void Scene_AddHoveredButton(std::shared_ptr<Button> button);
		void Scene_RemoveHoveredButton(std::shared_ptr<Button> button);
		std::shared_ptr<Button> Scene_GetTopLevelButton();
		std::vector<std::shared_ptr<Button>> Scene_GetHoveredButtons();

		// Game Canvases
		std::shared_ptr<Canvas> Game_CreateCanvas(long ID, long parentID, int layerNumber);
		void Game_DeleteCanvas(long ID);
		std::vector<std::shared_ptr<Canvas>> Game_GetCanvases();
		void Game_IncrementCanvasID();
		long Game_GetNextCanvasID();

		// Game Buttons
		std::shared_ptr<Button> Game_CreateButton(long ID, long parentID, int layerNumber);
		void Game_DeleteButton(long ID);
		void Game_AddHoveredButton(std::shared_ptr<Button> button);
		void Game_RemoveHoveredButton(std::shared_ptr<Button> button);
		std::shared_ptr<Button> Game_GetTopLevelButton();
		std::vector<std::shared_ptr<Button>> Game_GetHoveredButtons();

	private:
		long nextSceneCanvasID;
		long nextGameCanvasID;
		std::vector<std::shared_ptr<Button>> sceneButtons;
		std::vector<std::shared_ptr<Button>> gameButtons;
		std::vector<std::shared_ptr<Button>> sceneHoveredButtons;
		std::vector<std::shared_ptr<Button>> gameHoveredButtons;
		std::vector<std::shared_ptr<Canvas>> sceneCanvases;
		std::vector<std::shared_ptr<Canvas>> gameCanvases;
	};
}
}
