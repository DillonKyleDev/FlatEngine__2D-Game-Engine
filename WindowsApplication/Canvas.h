#pragma once
#include "Button.h"
#include "Component.h"


namespace FlatEngine 
{
	class Canvas : public Component
	{
	public:
		Canvas(long myID, long parentID);
		~Canvas();

		void AddButton(FlatEngine::Button* button);
		void RemoveButton(FlatEngine::Button* button);
		float GetWidth();
		float GetHeight();
		void SetDimensions(float width, float height);
		void SetLayerNumber(int layerNumber);
		int GetLayerNumber();
		void SetBlocksLayers(bool _blocksLayers);\
		bool GetBlocksLayers();
		std::vector<FlatEngine::Button*> GetButtons();
		std::string GetData();


	private:
		long canvasID;
		// Buttons a Canvas owns will be established at run time
		std::vector<FlatEngine::Button*> buttons;
		int layerNumber;
		bool _blocksLayers;
		float width;
		float height;
	};
}

