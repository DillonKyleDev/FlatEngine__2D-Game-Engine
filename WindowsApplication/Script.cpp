#include "Script.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>


namespace FlatEngine
{
	Script::Script()
	{
		this->SetType(Component::ComponentTypes::Script);
		this->path = "";
	}

	Script::~Script()
	{

	}

	std::string Script::CreateScriptFile(std::string name)
	{
		// Includes file
		std::ofstream inludesFile;

		// Include for the new script header file
		std::string includeString = "#include <scripts/headers/" + name + ".h> \n";

		// Open ScriptIncludes.cpp in append mode
		inludesFile.open("ScriptIncludes.h", std::ios::app);
		inludesFile << includeString.c_str() << std::endl;
		inludesFile.close();

		// New script .h and .cpp files
		std::ofstream newScriptCPP;
		std::ofstream newScriptH;

		// Boilerplate for the new script .cpp file
		std::string boilerplateCPP = "";
		// Boilerplate for the new script .cpp file
		std::string boilerplateH = "";

		// Create the new script .cpp and .h files
		newScriptCPP.open("scripts/cpp/" + name + ".cpp", std::ios::app);
		newScriptCPP << boilerplateCPP.c_str() << std::endl;
		newScriptCPP.close();

		newScriptH.open("scripts/headers/" + name + ".h", std::ios::app);
		newScriptH << boilerplateH.c_str() << std::endl;
		newScriptH.close();

		return name + ".cpp";
	}

	void Script::SetPath(std::string path)
	{
		this->path = path;
	}

	std::string Script::GetPath()
	{
		return this->path;
	}

	bool Script::IsActive()
	{
		return this->_isActive;
	}

	void Script::SetActive(bool _isActive)
	{
		this->_isActive = _isActive;
	}


	std::string Script::GetData()
	{
		json jsonData = {
			{ "type", "Script" },
			{ "path", this->path },
			{ "_isActive", this->_isActive }
		};

		std::string data = jsonData.dump();
		// Return dumped json object with required data for saving
		return data;
	}
}