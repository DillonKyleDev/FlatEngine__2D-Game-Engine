#pragma once
#include "../GameScript.h"

class GroundedCheck : public FlatEngine::GameScript
{
	class BoxCollider;
	class CharacterController;

public:
	GroundedCheck();
	~GroundedCheck();

	void Start();
	void Update(int deltaTime);

private:
	std::shared_ptr<FlatEngine::BoxCollider> collider;
	std::shared_ptr<FlatEngine::CharacterController> controller;
};