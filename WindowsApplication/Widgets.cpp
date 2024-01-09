#include "Widgets.h"


namespace FlatEngine { namespace FlatGui
{
	Widgets::Widgets()
	{
		this->hoveredButtons = {};
	}

	Widgets::~Widgets()
	{

	}

	void Widgets::AddHoveredButton(Button* button)
	{
		this->hoveredButtons.push_back(button);
	}

	void Widgets::RemoveHoveredButton(Button* button)
	{
		for (int i = 0; i < this->hoveredButtons.size(); i++)
		{
			if (this->hoveredButtons[i])
			{

			}
		}
	}

	void Widgets::IncrementID()
	{
		this->currentID++;
	}

	long Widgets::GetCurrentID()
	{
		return this->currentID;
	}

	Button* Widgets::GetTopLevelButton()
	{
		Button* highestPriority = nullptr;

		for (int i = 0; i < this->hoveredButtons.size(); i++)
		{
			if (i == 0)
				highestPriority = this->hoveredButtons[i];
			else if (this->hoveredButtons[i]->GetActiveLayer() > highestPriority->GetActiveLayer())
			{
				highestPriority = this->hoveredButtons[i];
			}
		}

		return highestPriority;
	}

	std::vector<Button*> Widgets::GetHoveredButtons()
	{
		return std::vector<Button*>();
	}
}
}