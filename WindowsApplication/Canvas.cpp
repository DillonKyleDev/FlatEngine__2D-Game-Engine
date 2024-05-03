#include "Canvas.h"
#include "FlatEngine.h"


namespace FlatEngine {
	Canvas::Canvas(long myID, long parentID, long canvasID)
	{
		SetType(ComponentTypes::Canvas);
		SetID(myID);
		SetParentID(parentID);
		canvasID = canvasID;
		buttons = std::vector<std::shared_ptr<FlatEngine::Button>>();
		layerNumber = canvasID;
		_blocksLayers = true;
		width = 50;
		height = 30;
	}

	Canvas::Canvas(std::shared_ptr<Canvas> toCopy, long newParentID)
	{
		SetType(ComponentTypes::Canvas);
		SetID(GetNextComponentID());
		SetParentID(newParentID);
		SetActive(toCopy->IsActive());
		canvasID = toCopy->canvasID;
		buttons = toCopy->GetButtons();
		layerNumber = toCopy->GetLayerNumber();
		_blocksLayers = toCopy->GetBlocksLayers();
		width = toCopy->GetWidth();
		height = toCopy->GetHeight();
	}

	Canvas::~Canvas()
	{
	}

	void Canvas::AddButton(std::shared_ptr<FlatEngine::Button> button)
	{
		buttons.push_back(button);
	}

	void Canvas::RemoveButton(std::shared_ptr<FlatEngine::Button> button)
	{
		for (int i = 0; i < buttons.size(); i++)
		{
			if (button->GetID() == buttons[i]->GetID())
				buttons.erase(buttons.begin() + i);
		}
	}

	float Canvas::GetWidth()
	{
		return width;
	}

	float Canvas::GetHeight()
	{
		return height;
	}

	void Canvas::SetDimensions(float newWidth, float newHeight)
	{
		if (newWidth >= 0 && newHeight >= 0)
		{
			width = newWidth;
			height = newHeight;
		}
		else
			FlatEngine::LogString("Canvas::SetDimensions() - Canvas width and height must be positive values.");
	}

	void Canvas::SetLayerNumber(int newLayerNumber)
	{
		layerNumber = newLayerNumber;
	}

	int Canvas::GetLayerNumber()
	{
		return layerNumber;
	}

	void Canvas::SetBlocksLayers(bool _doesBlock)
	{
		_blocksLayers = _doesBlock;
	}

	bool Canvas::GetBlocksLayers()
	{
		return _blocksLayers;
	}

	std::vector<std::shared_ptr<FlatEngine::Button>> Canvas::GetButtons()
	{
		return std::vector<std::shared_ptr<FlatEngine::Button>>();
	}

	std::string Canvas::GetData()
	{
		json jsonData = {
			{ "type", "Canvas" },
			{ "id", GetID() },
			{ "_isCollapsed", IsCollapsed() },
			{ "_isActive", IsActive() },
			{ "width", width },
			{ "height", height },
			{ "layerNumber", layerNumber },
			{ "_blocksLayers", _blocksLayers },
		};

		std::string data = jsonData.dump();
		// Return dumped json object with required data for saving
		return data;
	}
}