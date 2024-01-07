#pragma once



namespace FlatEngine
{
	class Settings
	{
	public:
		Settings();
		~Settings();

		void SaveSettings();
		void LoadSettings();
		void SetIconAlpha(float alpha);
		float GetIconAlpha();

	private:
		int iconAlpha;
		
	};
}


