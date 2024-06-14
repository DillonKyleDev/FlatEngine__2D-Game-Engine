#pragma once
#include "FlatEngine.h"
#include "GameScript.h"

class PlayerController : public FlatEngine::GameScript
{
	class MappingContext;
	class CharacterController;
	class RigidBody;
	class Transform;
	class Audio;

public:
	PlayerController(long ownerID);
	~PlayerController();

	void Start();
	void Update(float deltaTime);

private:
	std::shared_ptr<FlatEngine::MappingContext> mappingContext;
	std::shared_ptr<FlatEngine::CharacterController> characterController;
	std::shared_ptr<FlatEngine::RigidBody> rigidBody;
	std::shared_ptr<FlatEngine::Transform> transform;
	std::shared_ptr<FlatEngine::Audio> audio;
};