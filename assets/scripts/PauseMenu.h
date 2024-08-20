#pragma once
#include "FlatEngine.h"
#include "GameScript.h"

class PauseMenu : public FlatEngine::GameScript
{
	class MappingContext;
	class GameObject;

public:
	PauseMenu(long ownerID);
	~PauseMenu();

	void Start();
	void Update(float deltaTime);

private:
	std::shared_ptr<FlatEngine::MappingContext> mappingContext;
	std::shared_ptr<FlatEngine::GameObject> pauseScreen;
};

