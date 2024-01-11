#include "Canvas.h"
#include "FlatEngine.h"


namespace FlatEngine {
	Canvas::Canvas(long myID, long parentID)
	{
		this->SetType(ComponentTypes::Canvas);
		this->SetID(myID);
		this->SetParentID(parentID);
		this->canvasID = canvasID;
		this->buttons = std::vector<FlatEngine::Button*>();
		this->layerNumber = canvasID;
		this->_blocksLayers = true;
		this->width = 50;
		this->height = 30;
	}

	Canvas::~Canvas()
	{
	}

	void Canvas::AddButton(FlatEngine::Button* button)
	{
		this->buttons.push_back(button);
	}

	void Canvas::RemoveButton(FlatEngine::Button* button)
	{
		for (int i = 0; i < this->buttons.size(); i++)
		{
			if (button->GetID() == this->buttons[i]->GetID())
				this->buttons.erase(this->buttons.begin() + i);
		}
	}

	float Canvas::GetWidth()
	{
		return this->width;
	}

	float Canvas::GetHeight()
	{
		return this->height;
	}

	void Canvas::SetDimensions(float width, float height)
	{
		if (width >= 0 && height >= 0)
		{
			this->width = width;
			this->height = height;
		}
		else
			FlatEngine::LogString("Canvas::SetDimensions() - Canvas width and height must be positive values.");
	}

	void Canvas::SetLayerNumber(int layerNumber)
	{
		this->layerNumber = layerNumber;
	}

	int Canvas::GetLayerNumber()
	{
		return this->layerNumber;
	}

	void Canvas::SetBlocksLayers(bool _blocksLayers)
	{
		this->_blocksLayers = _blocksLayers;
	}

	bool Canvas::GetBlocksLayers()
	{
		return this->_blocksLayers;
	}

	std::vector<FlatEngine::Button*> Canvas::GetButtons()
	{
		return std::vector<FlatEngine::Button*>();
	}

	std::string Canvas::GetData()
	{
		json jsonData = {
			{ "type", "Canvas" },
			{ "id", this->GetID() },
			{ "_isCollapsed", this->IsCollapsed() },
			{ "width", this->width },
			{ "height", this->height },
			{ "layerNumber", this->layerNumber },
			{ "_blocksLayers", this->_blocksLayers },
		};

		std::string data = jsonData.dump();
		// Return dumped json object with required data for saving
		return data;
	}
}