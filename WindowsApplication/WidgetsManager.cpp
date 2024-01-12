#include "WidgetsManager.h"


namespace FlatEngine { namespace FlatGui
{
	WidgetsManager::WidgetsManager()
	{
		this->nextSceneCanvasID = 0;
		this->nextGameCanvasID = 0;
		this->sceneButtons = std::vector<std::shared_ptr<Button>>();
		this->gameButtons = std::vector<std::shared_ptr<Button>>();
		this->sceneHoveredButtons = std::vector<std::shared_ptr<Button>>();
		this->gameHoveredButtons = std::vector<std::shared_ptr<Button>>();
		this->sceneCanvases = std::vector<std::shared_ptr<Canvas>>();
		this->gameCanvases = std::vector<std::shared_ptr<Canvas>>();
	}

	WidgetsManager::~WidgetsManager()
	{
	}


	// Scene canvases are not connected to the main Components system. They are controlled completely by the WidgetsManager and
	// The Scene and SceneManager class have no idea they exist.
	std::shared_ptr<Canvas> WidgetsManager::Scene_CreateCanvas(int layerNumber)
	{
		std::shared_ptr<Canvas> newSceneCanvas(new Canvas(-1, -1, this->nextSceneCanvasID));
		newSceneCanvas->SetLayerNumber(layerNumber);
		this->sceneCanvases.push_back(newSceneCanvas);
		this->Scene_IncrementCanvasID();

		return newSceneCanvas;
	}

	void WidgetsManager::Scene_DeleteCanvas(long ID)
	{
		for (int i = 0; i < this->sceneCanvases.size(); i++)
		{
			if (this->sceneCanvases[i]->GetID() == ID)
			{
				this->sceneCanvases.erase(this->sceneCanvases.begin() + i);
			}
		}
	}

	std::vector<std::shared_ptr<Canvas>> WidgetsManager::Scene_GetCanvases()
	{
		return this->sceneCanvases;
	}

	void WidgetsManager::Scene_IncrementCanvasID()
	{
		this->nextSceneCanvasID++;
	}

	long WidgetsManager::Scene_GetNextCanvasID()
	{
		return this->nextSceneCanvasID;
	}

	std::shared_ptr<Button> WidgetsManager::Scene_CreateButton(long ID, long parentID, int layerNumber)
	{
		return nullptr;
	}

	void WidgetsManager::Scene_DeleteButton(long ID)
	{
		
	}

	void WidgetsManager::Scene_AddHoveredButton(std::shared_ptr<Button> button)
	{
		this->sceneHoveredButtons.push_back(button);
	}

	void WidgetsManager::Scene_RemoveHoveredButton(std::shared_ptr<Button> button)
	{
		for (int i = 0; i < this->sceneHoveredButtons.size(); i++)
		{
			if (this->sceneHoveredButtons[i]->GetID() == button->GetID())
			{
				this->sceneHoveredButtons.erase(this->sceneHoveredButtons.begin() + i);
			}
		}
	}

	std::shared_ptr<Button> WidgetsManager::Scene_GetTopLevelButton()
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

	std::vector<std::shared_ptr<Button>> WidgetsManager::Scene_GetHoveredButtons()
	{
		return this->sceneHoveredButtons;
	}

	std::shared_ptr<Canvas> WidgetsManager::Game_CreateCanvas(long componentID, long parentID, int layerNumber)
	{
		std::shared_ptr<Canvas> newSceneCanvas(new Canvas(componentID, parentID, this->nextSceneCanvasID));
		newSceneCanvas->SetLayerNumber(layerNumber);
		this->sceneCanvases.push_back(newSceneCanvas);
		this->Scene_IncrementCanvasID();

		return newSceneCanvas;
	}

	void WidgetsManager::Game_DeleteCanvas(long ID)
	{
		
	}

	std::vector<std::shared_ptr<Canvas>> WidgetsManager::Game_GetCanvases()
	{
		return this->gameCanvases;
	}

	void WidgetsManager::Game_IncrementCanvasID()
	{
		this->nextGameCanvasID++;
	}

	long WidgetsManager::Game_GetNextCanvasID()
	{
		return this->nextGameCanvasID;
	}

	std::shared_ptr<Button> WidgetsManager::Game_CreateButton(long ID, long parentID, int layerNumber)
	{
		return nullptr;
	}

	void WidgetsManager::Game_DeleteButton(long ID)
	{
	}

	void WidgetsManager::Game_AddHoveredButton(std::shared_ptr<Button> button)
	{
		this->gameHoveredButtons.push_back(button);
	}

	void WidgetsManager::Game_RemoveHoveredButton(std::shared_ptr<Button> button)
	{
		for (int i = 0; i < this->gameHoveredButtons.size(); i++)
		{
			if (this->gameHoveredButtons[i]->GetID() == button->GetID())
				this->gameHoveredButtons.erase(this->gameHoveredButtons.begin() + i);
		}
	}

	std::shared_ptr<Button> WidgetsManager::Game_GetTopLevelButton()
	{
		return nullptr;
	}

	std::vector<std::shared_ptr<Button>> WidgetsManager::Game_GetHoveredButtons()
	{
		return this->gameHoveredButtons;
	}
}
}