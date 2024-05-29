#pragma once
#include "../GameScript.h"


class JumpPad : public FlatEngine::GameScript
{
	class BoxCollider;

public:
	JumpPad();
	~JumpPad();

	void Start();
	void Update(int deltaTime);
	std::shared_ptr<FlatEngine::BoxCollider> boxCollider;
};

