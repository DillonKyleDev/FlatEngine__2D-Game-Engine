#pragma once
#include "Component.h"
#include "GameScript.h"


namespace FlatEngine
{
	class ScriptComponent : public Component
	{
	public:
		ScriptComponent(long myID = -1, long parentID = -1);
		~ScriptComponent();

		std::string GetData();
		void SetAttachedScript(std::string script);
		std::string GetAttachedScript();
		void SetScriptInstance(std::shared_ptr<GameScript> script);
		std::shared_ptr<GameScript> GetScriptInstance();
		bool IsActive();
		void SetActive(bool _isActive);

	private:
		std::string attachedScript;
		std::shared_ptr<GameScript> scriptInstance;
		bool _isActive;
	};
}
