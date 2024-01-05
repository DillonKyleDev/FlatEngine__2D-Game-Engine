#include "GameScript.h"



namespace FlatEngine
{
	GameScript::GameScript()
	{
		this->entities = {};
		this->_isActive = true;
	}

	GameScript::~GameScript()
	{

	}	

	void GameScript::AddEntity(GameObject* entity)
	{
		this->entities.push_back(entity);
	}

	std::vector<GameObject*> GameScript::GetEntities()
	{
		return this->entities;
	}
}
