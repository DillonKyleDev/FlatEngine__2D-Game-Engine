#pragma once
#include "Component.h"
#include "GameScript.h"


namespace FlatEngine
{
	class ScriptComponent : public Component
	{
	public:
		ScriptComponent(long myID = -1, long parentID = -1);
		ScriptComponent(ScriptComponent* toCopy, long newParentID, long myID = -1);
		~ScriptComponent();

		std::string GetData();
		void SetAttachedScript(std::string script);
		std::string GetAttachedScript();
		void SetScriptInstance(GameScript* script);
		GameScript* GetScriptInstance();

	private:
		std::string attachedScript;
		GameScript* scriptInstance;
	};
}
