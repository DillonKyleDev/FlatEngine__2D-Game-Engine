#include "Widgets.h"


namespace FlatEngine { namespace FlatGui
{
	Widgets::Widgets()
	{
		this->sceneHoveredButtons = std::vector<Button*>();
		this->gameHoveredButtons = std::vector<Button*>();
		this->sceneCanvases = std::vector<Canvas*>();
		this->gameCanvases = std::vector<Canvas*>();
	}

	Widgets::~Widgets()
	{

	}

	void Widgets::AddSceneCanvas(Canvas* canvas)
	{
	}

	void Widgets::RemoveSceneCanvas(Canvas* canvas)
	{
	}

	std::vector<Canvas*> Widgets::GetSceneCanvases()
	{
		return std::vector<Canvas*>();
	}

	void Widgets::AddHoveredSceneButton(Button* button)
	{
		this->sceneHoveredButtons.push_back(button);
	}

	void Widgets::RemoveHoveredSceneButton(Button* button)
	{
		for (int i = 0; i < this->sceneHoveredButtons.size(); i++)
		{
			if (this->sceneHoveredButtons[i])
			{
				this->sceneHoveredButtons.erase(this->sceneHoveredButtons.begin() + i);
			}
		}
	}

	void Widgets::IncrementSceneCanvasID()
	{
		this->nextSceneCanvasID++;
	}

	long Widgets::GetNextSceneCanvasID()
	{
		return this->nextSceneCanvasID;
	}

	Button* Widgets::GetTopLevelSceneButton()
	{
		// Figure this out later **********************
		Button* highestPriority = nullptr;

		for (int i = 0; i < this->sceneHoveredButtons.size(); i++)
		{
			if (i == 0)
				highestPriority = this->sceneHoveredButtons[i];
			else if (this->sceneHoveredButtons[i]->GetActiveLayer() > highestPriority->GetActiveLayer())
			{
				highestPriority = this->sceneHoveredButtons[i];
			}
		}

		return highestPriority;
	}

	std::vector<Button*> Widgets::GetHoveredSceneButtons()
	{
		// TO DO
		return std::vector<Button*>();
	}

	void Widgets::AddHoveredGameButton(Button* button)
	{
		this->gameHoveredButtons.push_back(button);
	}

	void Widgets::RemoveHoveredGameButton(Button* button)
	{
		for (int i = 0; i < this->gameHoveredButtons.size(); i++)
		{
			if (this->gameHoveredButtons[i]->GetID() == button->GetID())
				this->gameHoveredButtons.erase(this->gameHoveredButtons.begin() + i);
		}
	}

	void Widgets::IncrementGameCanvasID()
	{
		this->nextGameCanvasID++;
	}

	long Widgets::GetNextGameCanvasID()
	{
		return this->nextGameCanvasID;
	}

	Button* Widgets::GetTopLevelGameButton()
	{
		// TO DO
		return nullptr;
	}

	std::vector<Button*> Widgets::GetHoveredGameButtons()
	{
		return this->gameHoveredButtons;
	}
}
}