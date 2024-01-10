#pragma once
#include "Button.h"


namespace FlatEngine { namespace FlatGui 
{
	class Canvas
	{
	public:
		Canvas(long canvasID);
		~Canvas();

		void AddButton(Button* button);
		void RemoveButton(Button* button);
		std::vector<Button*> GetButtons();


	private:
		long canvasID;
		std::vector<Button*> buttons;
		int layerNumber;
		bool _blocksLayers;
	};
}
}


