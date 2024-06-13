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

	void GameScript::SetOwner(std::shared_ptr<GameObject> newOwner)
	{
		owner = newOwner;
	}

	std::shared_ptr<GameObject> GameScript::GetOwner()
	{
		return this->owner;
	}
}
