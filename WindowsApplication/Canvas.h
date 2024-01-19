#pragma once
#include "Button.h"
#include "Component.h"


namespace FlatEngine 
{
	class Canvas : public Component
	{
	public:
		Canvas(long myID = -1, long parentID = -1, long canvasID = -1);
		~Canvas();

		void AddButton(std::shared_ptr<FlatEngine::Button> button);
		void RemoveButton(std::shared_ptr<FlatEngine::Button> button);
		float GetWidth();
		float GetHeight();
		void SetDimensions(float width, float height);
		void SetLayerNumber(int layerNumber);
		int GetLayerNumber();
		void SetBlocksLayers(bool _blocksLayers);
		bool GetBlocksLayers();
		std::vector<std::shared_ptr<FlatEngine::Button>> GetButtons();
		std::string GetData();

	private:
		long canvasID;
		// Buttons a Canvas owns will be established at run time
		std::vector<std::shared_ptr<FlatEngine::Button>> buttons;
		int layerNumber;
		bool _blocksLayers;
		float width;
		float height;
	};
}

