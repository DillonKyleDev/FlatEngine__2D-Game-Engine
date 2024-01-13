#include "WidgetsManager.h"


namespace FlatEngine { namespace FlatGui
{
	WidgetsManager::WidgetsManager()
	{
		this->nextSceneCanvasID = 0;
		this->sceneButtons = std::vector<std::shared_ptr<Button>>();
		this->sceneHoveredButtons = std::vector<std::shared_ptr<Button>>();
		this->sceneCanvases = std::vector<std::shared_ptr<Canvas>>();
	}

	WidgetsManager::~WidgetsManager()
	{
	}


	// Scene canvases are not connected to the main Components system. They are controlled completely by the WidgetsManager and
	// The Scene and SceneManager class have no idea they exist.
	std::shared_ptr<Canvas> WidgetsManager::CreateCanvas(int layerNumber)
	{
		std::shared_ptr<Canvas> newSceneCanvas(new Canvas(-1, -1, this->nextSceneCanvasID));
		newSceneCanvas->SetLayerNumber(layerNumber);
		this->sceneCanvases.push_back(newSceneCanvas);
		this->IncrementCanvasID();

		return newSceneCanvas;
	}

	void WidgetsManager::DeleteCanvas(long ID)
	{
		for (int i = 0; i < this->sceneCanvases.size(); i++)
		{
			if (this->sceneCanvases[i]->GetID() == ID)
			{
				this->sceneCanvases.erase(this->sceneCanvases.begin() + i);
			}
		}
	}

	std::vector<std::shared_ptr<Canvas>> WidgetsManager::GetCanvases()
	{
		return this->sceneCanvases;
	}

	void WidgetsManager::IncrementCanvasID()
	{
		this->nextSceneCanvasID++;
	}

	long WidgetsManager::GetNextCanvasID()
	{
		return this->nextSceneCanvasID;
	}

	std::shared_ptr<Button> WidgetsManager::CreateButton(long ID, long parentID, int layerNumber)
	{
		return nullptr;
	}

	void WidgetsManager::DeleteButton(long ID)
	{
		
	}

	void WidgetsManager::AddHoveredButton(std::shared_ptr<Button> button)
	{
		this->sceneHoveredButtons.push_back(button);
	}

	void WidgetsManager::RemoveHoveredButton(std::shared_ptr<Button> button)
	{
		for (int i = 0; i < this->sceneHoveredButtons.size(); i++)
		{
			if (this->sceneHoveredButtons[i]->GetID() == button->GetID())
			{
				this->sceneHoveredButtons.erase(this->sceneHoveredButtons.begin() + i);
			}
		}
	}

	std::shared_ptr<Button> WidgetsManager::GetTopLevelButton()
	{
		// Figure this out later **********************
		long highestPriorityID = -1;

		/*for (int i = 0; i < this->sceneHoveredButtons.size(); i++)
		{
			if (i == 0)
				highestPriorityID = this->sceneHoveredButtons[i]->GetID();
			else if (this->sceneHoveredButtons[i]->GetActiveLayer() > highestPriority->GetActiveLayer())
			{
				highestPriority = this->sceneHoveredButtons[i];
			}
		}*/

		return nullptr;
	}

	std::vector<std::shared_ptr<Button>> WidgetsManager::GetHoveredButtons()
	{
		return this->sceneHoveredButtons;
	}
}
}