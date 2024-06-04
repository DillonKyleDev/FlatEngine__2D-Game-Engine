#pragma once
#include "../FlatEngine.h"
#include "../GameScript.h"

class PlayerController : public FlatEngine::GameScript
{
	class MappingContext;
	class CharacterController;
	class RigidBody;
	class Transform;

public:
	PlayerController();
	~PlayerController();

	void Start();
	void Update(float deltaTime);

private:
	std::shared_ptr<FlatEngine::MappingContext> mappingContext;
	std::shared_ptr<FlatEngine::CharacterController> characterController;
	std::shared_ptr<FlatEngine::RigidBody> rigidBody;
	std::shared_ptr<FlatEngine::Transform> transform;
};