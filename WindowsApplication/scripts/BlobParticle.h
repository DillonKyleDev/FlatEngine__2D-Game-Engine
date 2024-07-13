#pragma once
#include "../FlatEngine.h"
#include "../GameScript.h"


class BlobParticle : public FlatEngine::GameScript
{
	class FlatEngineGameObject;
public:
	BlobParticle(long ownerID);
	~BlobParticle();

	void Start();
	void Update(float deltaTime);
	void LogObjectName();
	FlatEngine::GameObject & GetTestObject();

private:
	FlatEngine::GameObject m_TestObject;
};

