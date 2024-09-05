#include "UIManager.h"
#include "FlatEngine.h"
#include "GameObject.h"
#include "Transform.h"

#include "imgui.h"
#include "imgui_internal.h"
#include <algorithm>
#include <memory>

namespace FlatEngine
{
	using Transform = FlatEngine::Transform;
	using GameObject = FlatEngine::GameObject;

	UIManager::UIManager()
	{
		this->nextGameCanvasID = 0;
		this->gameButtons = std::vector<Button*>();
		this->gameHoveredButtons = std::vector<Button*>();
		this->gameCanvases = std::vector<Canvas*>();
	}

	UIManager::~UIManager()
	{
	}


	Canvas UIManager::CreateCanvas(long ID, long parentID, int layerNumber)
	{
		Canvas newGameCanvas = Canvas(ID, parentID, nextGameCanvasID);
		newGameCanvas.SetLayerNumber(layerNumber);
		this->gameCanvases.push_back(&newGameCanvas);
		this->IncrementCanvasID();

		return newGameCanvas;
	}

	void UIManager::DeleteCanvas(long ID)
	{
		for (int i = 0; i < this->gameCanvases.size(); i++)
		{
			if (this->gameCanvases[i]->GetID() == ID)
			{
				this->gameCanvases.erase(this->gameCanvases.begin() + i);
			}
		}
	}

	std::vector<Canvas*> UIManager::GetCanvases()
	{
		return gameCanvases;
	}

	void UIManager::IncrementCanvasID()
	{
		nextGameCanvasID++;
	}

	long UIManager::GetNextCanvasID()
	{
		return nextGameCanvasID;
	}

	bool UIManager::CheckForMouseOver()
	{
		// Save a copy of the last hovered buttons to check if we should fire onMouseLeave()
		std::vector<Button*> lastHovered = gameHoveredButtons;
		// Clear the hovered buttons vector
		gameHoveredButtons = std::vector<Button*>();

		for (int i = 0; i < gameButtons.size(); i++)
		{
			if (gameButtons[i]->IsActive())
			{
				// Get the Buttons Transform data
				GameObject *thisObject = FlatEngine::GetObjectById(gameButtons[i]->GetParentID());
				Transform* transform = nullptr;
				if (thisObject != nullptr)
					transform = thisObject->GetTransform();

				// Get the buttons active edges
				Button* button = gameButtons[i];
				ImVec4 activeEdges = button->GetActiveEdges();

				// Get Mouse Position in the viewport space
				ImVec2 mousePos = ImGui::GetIO().MousePos;	
					
				// If Mouse and Button are colliding, add the hovered button
				if (FlatEngine::AreCollidingViewport(activeEdges, ImVec4(mousePos.y, mousePos.x, mousePos.y, mousePos.x)))
				{
					this->AddHoveredButton(button);

					if (button->MouseOverSet())
					{
						GameObject *thisObject = FlatEngine::GetObjectById(button->GetParentID());
						button->OnMouseOverFunction(thisObject);
					}
				}
			}
		}

		// For Mouse Enter function
		for (int i = 0; i < this->gameHoveredButtons.size(); i++)
		{
			// If the new vector has an item that's new from the last saved vector, fire OnMouseEnter() on that item
			if (std::find(lastHovered.begin(), lastHovered.end(), this->gameHoveredButtons[i]) == lastHovered.end())
			{
				if (this->gameHoveredButtons[i]->MouseEnterSet())
				{
					GameObject *thisObject = FlatEngine::GetObjectById(this->gameHoveredButtons[i]->GetParentID());
					this->gameHoveredButtons[i]->OnMouseEnterFunction(thisObject);
				}
			}
		}

		// For Mouse Leave function
		for (int i = 0; i < lastHovered.size(); i++)
		{
			// If the new vector is missing an item from the last saved vector, fire OnMouseLeave() on that item
			if (std::find(this->gameHoveredButtons.begin(), this->gameHoveredButtons.end(), lastHovered[i]) == this->gameHoveredButtons.end())
			{
				if (lastHovered.size() > 0 && lastHovered[i]->MouseLeaveSet())
				{
					GameObject *thisObject = FlatEngine::GetObjectById(lastHovered[i]->GetParentID());
					lastHovered[i]->OnMouseLeaveFunction(thisObject);
				}
			}
		}

		return this->gameHoveredButtons.size() > 0;
	}

	Canvas* UIManager::GetFirstUnblockedCanvas()
	{
		Canvas* lowestUnblockedCanvas = nullptr;
		int lowestUnblockedLayer = 0;

		for (int i = 0; i < this->gameCanvases.size(); i++)
		{
			Canvas* canvas = this->gameCanvases[i];
			GameObject *parentObject = FlatEngine::GetObjectById(canvas->GetParentID());

			// If parent object is active 
			// AND if the Canvas blocks layers below it 
			// AND if the layer is higher than the current lowestUnblockedLayer
			// make it the new lowest unblockedlayer
			bool _blocksLayer = canvas->GetBlocksLayers();
			int layerNumber = canvas->GetLayerNumber();
			if (parentObject != nullptr && _blocksLayer && layerNumber > lowestUnblockedLayer)
			{
				lowestUnblockedCanvas = canvas;
				lowestUnblockedLayer = canvas->GetLayerNumber();
			}
		}

		return lowestUnblockedCanvas;
	}

	int UIManager::GetFirstUnblockedLayer()
	{
		if (this->GetFirstUnblockedCanvas() != nullptr)
			return this->GetFirstUnblockedCanvas()->GetLayerNumber();
		else
			return -1;
	}

	Button UIManager::CreateButton(long ID, long parentID, int layerNumber)
	{
		FlatEngine::Button newGameButton = FlatEngine::Button(ID, parentID);
		newGameButton.SetActiveLayer(layerNumber);
		this->gameButtons.push_back(&newGameButton);
		this->IncrementCanvasID();

		return newGameButton;
	}

	void UIManager::DeleteButton(long ID)
	{
		for (int i = 0; i < this->gameButtons.size(); i++)
		{
			if (this->gameButtons[i]->GetID() == ID)
			{
				this->gameButtons.erase(this->gameButtons.begin() + i);
			}
		}
	}

	void UIManager::AddHoveredButton(Button* button)
	{
		this->gameHoveredButtons.push_back(button);
	}

	void UIManager::RemoveHoveredButton(Button* button)
	{
		for (int i = 0; i < this->gameHoveredButtons.size(); i++)
		{
			if (this->gameHoveredButtons[i]->GetID() == button->GetID())
				this->gameHoveredButtons.erase(this->gameHoveredButtons.begin() + i);
		}
	}

	Button* UIManager::GetTopLevelButton()
	{
		// If there is a blocking layer that is active on screen
		if (this->GetFirstUnblockedLayer() != -1)
		{
			for (int i = 0; i < this->gameHoveredButtons.size(); i++)
			{
				// If this button is on or above that blocking layer, return that button
				if (this->gameHoveredButtons[i]->GetActiveLayer() >= this->GetFirstUnblockedLayer())
				{
					return this->gameHoveredButtons[i];
				}
			}
			// If no button was above the lowest unblocked layer, return nullptr
			return nullptr;
		}
		// Else, if there isn't a blocking layer, there shouldn't be any overlapping buttons,
		// so just return the first hovered button
		else
			return this->gameHoveredButtons[0];
	}

	std::vector<Button* > UIManager::GetHoveredButtons()
	{
		return this->gameHoveredButtons;
	}
	void UIManager::ResetButtons()
	{
		this->gameHoveredButtons = std::vector<Button*>();
		this->gameButtons = std::vector<Button*>();
	}
}