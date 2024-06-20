#pragma once
#include "GameScript.h"
#include "Health.h"


class EnemyController : public FlatEngine::GameScript
{
public:
	EnemyController(long ownerID);
	~EnemyController();

	void Start();
	void Update(float deltaTime);

private:
	std::shared_ptr<Health> health;
	std::shared_ptr<FlatEngine::BoxCollider> boxCollider;
	std::vector<std::shared_ptr<FlatEngine::CircleCollider>> circleColliders;
	std::shared_ptr<FlatEngine::CompositeCollider> compositeCollider;
	std::shared_ptr<FlatEngine::RigidBody> rigidBody;
	std::shared_ptr<FlatEngine::Transform> transform;
	std::shared_ptr<FlatEngine::Animation> animator;
	std::shared_ptr<FlatEngine::Sprite> sprite;
	std::shared_ptr<FlatEngine::Audio> audio;
	std::shared_ptr<FlatEngine::GameObject> whipArm;
};