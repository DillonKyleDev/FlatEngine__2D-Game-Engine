#pragma once
#include "Component.h"
#include "GameScript.h"


namespace FlatEngine
{
	class ScriptComponent : public Component
	{
	public:
		ScriptComponent(long parentID);
		~ScriptComponent();

		std::string GetData();
		void SetAttachedScript(std::string script);
		std::string GetAttachedScript();
		bool IsActive();
		void SetActive(bool _isActive);

	private:
		std::string attachedScript;
		bool _isActive;
	};
}
