#include "ScriptComponent.h"
#include "FlatEngine.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>


namespace FlatEngine
{
	ScriptComponent::ScriptComponent(long myID, long parentID)
	{
		this->SetType(Component::ComponentTypes::Script);
		this->SetID(myID);
		this->SetParentID(parentID);
		this->_isActive = true;
		this->attachedScript = "";
		this->scriptInstance = nullptr;
	}

	ScriptComponent::ScriptComponent(std::shared_ptr<ScriptComponent> toCopy)
	{
		this->SetType(Component::ComponentTypes::Script);
		this->SetID(GetNextComponentID());
		this->SetParentID(toCopy->GetParentID());
		this->_isActive = toCopy->IsActive();
		this->attachedScript = toCopy->GetAttachedScript();
		this->scriptInstance = toCopy->GetScriptInstance();
	}

	ScriptComponent::~ScriptComponent()
	{

	}

	void ScriptComponent::SetAttachedScript(std::string script)
	{
		this->attachedScript = script;
	}

	std::string ScriptComponent::GetAttachedScript()
	{
		return this->attachedScript;
	}

	void ScriptComponent::SetScriptInstance(std::shared_ptr<GameScript> script)
	{
		this->scriptInstance = script;
	}

	std::shared_ptr<GameScript> ScriptComponent::GetScriptInstance()
	{
		return this->scriptInstance;
	}

	bool ScriptComponent::IsActive()
	{
		return this->_isActive;
	}

	void ScriptComponent::SetActive(bool _isActive)
	{
		this->_isActive = _isActive;
	}


	std::string ScriptComponent::GetData()
	{
		json jsonData = {
			{ "type", "Script" },
			{ "id", this->GetID() },
			{ "_isCollapsed", this->IsCollapsed() },
			{ "attachedScript", this->attachedScript },
			{ "_isActive", this->_isActive }
		};

		std::string data = jsonData.dump();
		// Return dumped json object with required data for saving
		return data;
	}
}




// Includes file
//std::ofstream inludesFile;
//
//// Include for the new script header file
//std::string includeString = "#include <scripts/headers/" + name + ".h> \n";
//
//// Open ScriptIncludes.cpp in append mode
//inludesFile.open("ScriptIncludes.h", std::ios::app);
//inludesFile << includeString.c_str() << std::endl;
//inludesFile.close();
//
//// New script .h and .cpp files
//std::ofstream newScriptCPP;
//std::ofstream newScriptH;
//
//// Boilerplate for the new script .cpp file
//std::string boilerplateCPP = "";
//// Boilerplate for the new script .cpp file
//std::string boilerplateH = "";
//
//// Create the new script .cpp and .h files
//newScriptCPP.open("scripts/cpp/" + name + ".cpp", std::ios::app);
//newScriptCPP << boilerplateCPP.c_str() << std::endl;
//newScriptCPP.close();
//
//newScriptH.open("scripts/headers/" + name + ".h", std::ios::app);
//newScriptH << boilerplateH.c_str() << std::endl;
//newScriptH.close();
//
//return name + ".cpp";