#pragma once
#pragma once
#include <vector>
#include "Canvas.h"
#include "Button.h"



namespace FlatEngine {
	namespace FlatGui
	{
		class UIManager
		{
		public:
			UIManager();
			~UIManager();

			// Canvases
			std::shared_ptr<Canvas> CreateCanvas(long ID, long parentID, int layerNumber);
			void DeleteCanvas(long ID);
			std::vector<std::shared_ptr<Canvas>> GetCanvases();
			void IncrementCanvasID();
			long GetNextCanvasID();
			bool CheckForMouseOver();
			std::shared_ptr<Canvas> GetFirstUnblockedCanvas();
			int GetFirstUnblockedLayer();

			// Buttons
			std::shared_ptr<Button> CreateButton(long ID, long parentID, int layerNumber);
			void DeleteButton(long ID);
			void AddHoveredButton(std::shared_ptr<Button> button);
			void RemoveHoveredButton(std::shared_ptr<Button> button);
			std::shared_ptr<Button> GetTopLevelButton();
			std::vector<std::shared_ptr<Button>> GetHoveredButtons();
			void ResetButtons();

		private:
			long nextGameCanvasID;
			std::vector<std::shared_ptr<Button>> gameButtons;
			std::vector<std::shared_ptr<Button>> gameHoveredButtons;
			std::vector<std::shared_ptr<Canvas>> gameCanvases;
		};
	}
}
