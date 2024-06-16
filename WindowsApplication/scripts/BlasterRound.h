#pragma once
#include "GameScript.h"


class BlasterRound : public FlatEngine::GameScript
{
	class BoxCollider;

public:
	BlasterRound(long ownerID);
	~BlasterRound();

	void Start();
	void Update(float deltaTime);
	std::shared_ptr<FlatEngine::CircleCollider> circleCollider;
	int startTime;
	int lifeLength;
};

