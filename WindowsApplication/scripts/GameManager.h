#pragma once
#include "../GameScript.h"

class GameManager : public FlatEngine::GameScript
{
public:
	GameManager();
	~GameManager();

	void Start();
	void Update(int deltaTime);
	void PauseGame();

private:
	bool _paused;
	std::shared_ptr<FlatEngine::GameObject> pauseScreen;
};

