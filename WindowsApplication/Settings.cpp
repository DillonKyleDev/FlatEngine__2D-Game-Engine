#include "Settings.h"


namespace FlatEngine
{
	Settings::Settings()
	{
		this->iconAlpha = 100;
	}

	Settings::~Settings()
	{

	}

	void Settings::SaveSettings()
	{

	}

	void Settings::LoadSettings()
	{

	}

	void Settings::SetIconAlpha(float alpha)
	{
		if (alpha >= 0 && alpha <= 255)
			this->iconAlpha = alpha;
	}

	float Settings::GetIconAlpha()
	{
		return this->iconAlpha;
	}
}