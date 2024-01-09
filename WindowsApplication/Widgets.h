#pragma once
#include <vector>
#include "Button.h"


namespace FlatEngine { namespace FlatGui
{
	class Widgets
	{
	public:
		Widgets();
		~Widgets();

		void AddHoveredButton(Button* button);
		void RemoveHoveredButton(Button* button);
		void IncrementID();
		long GetCurrentID();
		Button* GetTopLevelButton();
		std::vector<Button*> GetHoveredButtons();

	private:
		long currentID;
		std::vector<Button*> hoveredButtons;
	};
}
}
