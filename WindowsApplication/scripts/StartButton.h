#pragma once
#include "../GameScript.h"
#include "../Sound.h"


class StartButton : public FlatEngine::GameScript
{
public:
	StartButton();
	~StartButton();

	void Start();
	void Update(float deltaTime);

private:
	std::shared_ptr<FlatEngine::Sound> backgroundAudio;
};