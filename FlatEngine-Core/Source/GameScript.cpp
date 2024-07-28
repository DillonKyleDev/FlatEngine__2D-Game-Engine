#include "GameScript.h"



namespace FlatEngine
{
	GameScript::GameScript(long newOwnerID)
	{
		this->_isActive = true;
		ownerID = newOwnerID;
	}

	GameScript::~GameScript()
	{
	}

	void GameScript::SetName(std::string name)
	{
		this->name = name;
	}

	std::string GameScript::GetName()
	{
		return this->name;
	}

	long GameScript::GetOwnerID()
	{
		return ownerID;
	}

	void GameScript::SetOwnerID(long newID)
	{
		ownerID = newID;
	}

	void GameScript::SetOwner(GameObject *newOwner)
	{
		owner = newOwner;
	}

	GameObject* GameScript::GetOwner()
	{
		return owner;
	}
}
