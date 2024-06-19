#pragma once
#include "FlatEngine.h"
#include "GameScript.h"

class PlayerController : public FlatEngine::GameScript
{
	class MappingContext;
	class CharacterController;
	class BoxCollider;
	class RigidBody;
	class Transform;
	class Animation;
	class Sprite;
	class Audio;
	class GameObject;

public:
	PlayerController(long ownerID);
	~PlayerController();

	void Start();
	void Update(float deltaTime);

private:
	std::shared_ptr<FlatEngine::MappingContext> mappingContext;
	std::shared_ptr<FlatEngine::CharacterController> characterController;
	std::shared_ptr<FlatEngine::BoxCollider> boxCollider;
	std::shared_ptr<FlatEngine::RigidBody> rigidBody;
	std::shared_ptr<FlatEngine::Transform> transform;
	std::shared_ptr<FlatEngine::Animation> animator;
	std::shared_ptr<FlatEngine::Sprite> sprite;
	std::shared_ptr<FlatEngine::Audio> audio;
	std::shared_ptr<FlatEngine::GameObject> whipArm;
};