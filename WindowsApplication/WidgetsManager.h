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

		// Canvases
		std::shared_ptr<Canvas> CreateCanvas(int layerNumber);
		void DeleteCanvas(long ID);
		std::vector<std::shared_ptr<Canvas>> GetCanvases();
		void IncrementCanvasID();
		long GetNextCanvasID();

		// Buttons
		std::shared_ptr<Button> CreateButton(long ID, long parentID, int layerNumber);
		void DeleteButton(long ID);
		void AddHoveredButton(std::shared_ptr<Button> button);
		void RemoveHoveredButton(std::shared_ptr<Button> button);
		std::shared_ptr<Button> GetTopLevelButton();
		std::vector<std::shared_ptr<Button>> GetHoveredButtons();

	private:
		long nextSceneCanvasID;
		std::vector<std::shared_ptr<Button>> sceneButtons;
		std::vector<std::shared_ptr<Button>> sceneHoveredButtons;
		std::vector<std::shared_ptr<Canvas>> sceneCanvases;
	};
}
}
