#pragma once
#include "GameScript.h"


class BlobParticle : public FlatEngine::GameScript
{
public:
	BlobParticle(long ownerID);
	~BlobParticle();

	void Start();
	void Update(float deltaTime);

private:

};

