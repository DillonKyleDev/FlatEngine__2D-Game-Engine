#include "Canvas.h"


namespace FlatEngine { namespace FlatGui {
	FlatEngine::FlatGui::Canvas::Canvas(long canvasID)
	{
		this->canvasID = canvasID;
		this->buttons = std::vector<Button*>();
		this->layerNumber = canvasID;
		this->_blocksLayers = true;
	}

	FlatEngine::FlatGui::Canvas::~Canvas()
	{
	}

	void FlatEngine::FlatGui::Canvas::AddButton(Button* button)
	{
		this->buttons.push_back(button);
	}

	void FlatEngine::FlatGui::Canvas::RemoveButton(Button* button)
	{
		for (int i = 0; i < this->buttons.size(); i++)
		{
			if (button->GetID() == this->buttons[i]->GetID())
				this->buttons.erase(this->buttons.begin() + i);
		}
	}

	std::vector<FlatEngine::Button*> FlatEngine::FlatGui::Canvas::GetButtons()
	{
		return std::vector<Button*>();
	}
}
}