#pragma once
#include <vector>
#include "Canvas.h"
#include "Button.h"


namespace FlatEngine
{
	using Canvas = FlatEngine::Canvas;
	using Button = FlatEngine::Button;

	class UIManager
	{
	public:
		UIManager();
		~UIManager();

		// Canvases
		Canvas CreateCanvas(long ID, long parentID, int layerNumber);
		void DeleteCanvas(long ID);
		std::vector<Canvas*> GetCanvases();
		void IncrementCanvasID();
		long GetNextCanvasID();
		bool CheckForMouseOver();
		Canvas* GetFirstUnblockedCanvas();
		int GetFirstUnblockedLayer();

		// Buttons
		Button CreateButton(long ID, long parentID, int layerNumber);
		void DeleteButton(long ID);
		void AddHoveredButton(Button* button);
		void RemoveHoveredButton(Button* button);
		Button* GetTopLevelButton();
		std::vector<Button*> GetHoveredButtons();
		void ResetButtons();

	private:
		long nextGameCanvasID;
		std::vector<Button*> gameButtons;
		std::vector<Button*> gameHoveredButtons;
		std::vector<Canvas*> gameCanvases;
	};
}