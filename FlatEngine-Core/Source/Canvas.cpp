#include "Canvas.h"
#include "FlatEngine.h"


namespace FlatEngine 
{
	Canvas::Canvas(long myID, long parentID, long newCanvasID)
	{
		SetType(ComponentTypes::T_Canvas);
		SetID(myID);
		SetParentID(parentID);
		m_canvasID = newCanvasID;
		m_buttons = std::vector<std::shared_ptr<FlatEngine::Button>>();
		m_layerNumber = m_canvasID;
		m_b_blocksLayers = true;
		m_width = 50;
		m_height = 30;
	}

	Canvas::~Canvas()
	{
	}

	void Canvas::AddButton(std::shared_ptr<FlatEngine::Button> button)
	{
		m_buttons.push_back(button);
	}

	void Canvas::RemoveButton(std::shared_ptr<FlatEngine::Button> button)
	{
		for (int i = 0; i < m_buttons.size(); i++)
		{
			if (button->GetID() == m_buttons[i]->GetID())
				m_buttons.erase(m_buttons.begin() + i);
		}
	}

	float Canvas::GetWidth()
	{
		return m_width;
	}

	float Canvas::GetHeight()
	{
		return m_height;
	}

	void Canvas::SetDimensions(float newWidth, float newHeight)
	{
		if (newWidth >= 0 && newHeight >= 0)
		{
			m_width = newWidth;
			m_height = newHeight;
		}
		else
			FlatEngine::LogString("Canvas::SetDimensions() - Canvas width and height must be positive values.");
	}

	void Canvas::SetLayerNumber(int newLayerNumber)
	{
		m_layerNumber = newLayerNumber;
	}

	int Canvas::GetLayerNumber()
	{
		return m_layerNumber;
	}

	void Canvas::SetBlocksLayers(bool _doesBlock)
	{
		m_b_blocksLayers = _doesBlock;
	}

	bool Canvas::GetBlocksLayers()
	{
		return m_b_blocksLayers;
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
			{ "width", m_width },
			{ "height", m_height },
			{ "layerNumber", m_layerNumber },
			{ "_blocksLayers", m_b_blocksLayers },
		};

		std::string data = jsonData.dump();
		// Return dumped json object with required data for saving
		return data;
	}
}