#include "UIManager.h"
#include "FlatEngine.h"


namespace FlatEngine {
	namespace FlatGui
	{
		UIManager::UIManager()
		{
			this->nextGameCanvasID = 0;
			this->gameButtons = std::vector<std::shared_ptr<Button>>();
			this->gameHoveredButtons = std::vector<std::shared_ptr<Button>>();
			this->gameCanvases = std::vector<std::shared_ptr<Canvas>>();
		}

		UIManager::~UIManager()
		{
		}


		std::shared_ptr<Canvas> UIManager::CreateCanvas(long ID, long parentID, int layerNumber)
		{
			std::shared_ptr<Canvas> newGameCanvas(new Canvas(ID, parentID, this->nextGameCanvasID));
			newGameCanvas->SetLayerNumber(layerNumber);
			this->gameCanvases.push_back(newGameCanvas);
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

		std::vector<std::shared_ptr<Canvas>> UIManager::GetCanvases()
		{
			return this->gameCanvases;
		}

		void UIManager::IncrementCanvasID()
		{
			this->nextGameCanvasID++;
		}

		long UIManager::GetNextCanvasID()
		{
			return this->nextGameCanvasID;
		}

		bool UIManager::CheckForMouseOver()
		{
			// Clear the hovered buttons vector
			this->gameHoveredButtons = std::vector<std::shared_ptr<FlatEngine::Button>>();

			for (int i = 0; i < this->gameButtons.size(); i++)
			{
				if (this->gameButtons[i]->IsActive())
				{
					// Get the Buttons Transform data
					std::shared_ptr<GameObject> thisObject = FlatEngine::GetObjectById(this->gameButtons[i]->GetParentID());
					std::shared_ptr<FlatEngine::Transform> transform = std::static_pointer_cast<FlatEngine::Transform>(thisObject->GetComponent(FlatEngine::Component::ComponentTypes::Transform));
					Vector2 position = transform->GetPosition();
					float xPos = position.x;
					float yPos = position.y;

					// Get the buttons Button data
					std::shared_ptr<FlatEngine::Button> button = this->gameButtons[i];
					float buttonWidth = button->GetActiveWidth();
					float buttonHeight = button->GetActiveHeight();
					Vector2 offset = button->GetActiveOffset();

					float top = yPos + offset.y + buttonHeight / 2;
					float bottom = yPos + offset.y - buttonHeight / 2;
					float left = xPos + offset.x - buttonWidth / 2;
					float right = xPos + offset.x + buttonWidth / 2;

					// Get Mouse Position in world space
					ImVec2 mousePos = ImGui::GetIO().MousePos;
					ImVec2 m_posWorld = FlatEngine::FlatGui::ViewportToWorld(mousePos);

					// If Mouse and Button are colliding, add the hovered button
					if (FlatEngine::AreColliding(ImVec4(top, right, bottom, left), ImVec4(m_posWorld.y, m_posWorld.x, m_posWorld.y, m_posWorld.x)))
						this->AddHoveredButton(button);
				}
			}

			return this->gameHoveredButtons.size() > 0;
		}

		std::shared_ptr<Canvas> UIManager::GetFirstUnblockedCanvas()
		{
			std::shared_ptr<Canvas> lowestUnblockedCanvas = nullptr;
			int lowestUnblockedLayer = 0;

			for (int i = 0; i < this->gameCanvases.size(); i++)
			{
				std::shared_ptr<Canvas> canvas = this->gameCanvases[i];
				std::shared_ptr<GameObject> parentObject = FlatEngine::GetObjectById(canvas->GetParentID());

				// If parent object is active 
				// AND if the Canvas blocks layers below it 
				// AND if the layer is higher than the current lowestUnblockedLayer
				// make it the new lowest unblockedlayer
				bool _blocksLayer = canvas->GetBlocksLayers();
				int layerNumber = canvas->GetLayerNumber();
				if (parentObject->IsActive() && _blocksLayer && layerNumber > lowestUnblockedLayer)
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

		std::shared_ptr<Button> UIManager::CreateButton(long ID, long parentID, int layerNumber)
		{
			std::shared_ptr<FlatEngine::Button> newGameButton(new FlatEngine::Button(ID, parentID));
			newGameButton->SetActiveLayer(layerNumber);
			this->gameButtons.push_back(newGameButton);
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

		void UIManager::AddHoveredButton(std::shared_ptr<Button> button)
		{
			this->gameHoveredButtons.push_back(button);
		}

		void UIManager::RemoveHoveredButton(std::shared_ptr<Button> button)
		{
			for (int i = 0; i < this->gameHoveredButtons.size(); i++)
			{
				if (this->gameHoveredButtons[i]->GetID() == button->GetID())
					this->gameHoveredButtons.erase(this->gameHoveredButtons.begin() + i);
			}
		}

		std::shared_ptr<Button> UIManager::GetTopLevelButton()
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

		std::vector<std::shared_ptr<Button>> UIManager::GetHoveredButtons()
		{
			return this->gameHoveredButtons;
		}
	}
}