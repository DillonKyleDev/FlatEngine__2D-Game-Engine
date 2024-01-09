#pragma once
#include "Button.h"


namespace FlatEngine { namespace FlatGui 
{
	class Layer
	{
	public:
		Layer();
		~Layer();

		void AddButton(Button* button);
		void RemoveButton(Button* button);
		std::vector<Button*> GetButtons();


	private:
		std::vector<Button*> buttons;
		int layerNumber;
		bool _blocksLayers;
	};
}
}


