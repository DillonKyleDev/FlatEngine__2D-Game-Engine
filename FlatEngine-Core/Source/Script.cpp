#include "Script.h"
#include "FlatEngine.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>


namespace FlatEngine
{
	Script::Script(long myID, long parentID)
	{
		SetType(Component::ComponentTypes::T_Script);
		SetID(myID);
		SetParentID(parentID);
		m_attachedScript = "";
	}

	Script::~Script()
	{
	}

	void Script::SetAttachedScript(std::string script)
	{
		m_attachedScript = script;
	}

	std::string Script::GetAttachedScript()
	{
		return m_attachedScript;
	}

	std::string Script::GetData()
	{
		json jsonData = {
			{ "type", "Script" },
			{ "id", GetID() },
			{ "_isCollapsed", IsCollapsed() },
			{ "_isActive", IsActive() },
			{ "attachedScript", m_attachedScript },
		};

		std::string data = jsonData.dump();
		// Return dumped json object with required data for saving
		return data;
	}
}