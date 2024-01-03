#pragma once
#include "Component.h"


namespace FlatEngine
{
	class Script : public Component
	{
	public:
		Script();
		~Script();

		std::string GetData();
		std::string CreateScriptFile(std::string name);
		void SetPath(std::string path);
		std::string GetPath();
		bool IsActive();
		void SetActive(bool _isActive);

	private:
		std::string path;
		bool _isActive;
	};
}
