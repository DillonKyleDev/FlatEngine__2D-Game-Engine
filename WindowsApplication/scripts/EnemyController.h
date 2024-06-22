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
	void MoveToObject(std::shared_ptr<FlatEngine::GameObject> object);
	void AttackObject(std::shared_ptr<FlatEngine::GameObject> object);
	void SetSeesPlayer(bool _newSeesPlayer);
	void StartChaseTimeout();
	void StopChaseTimeout();

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

	// Attached Objects
	std::shared_ptr<FlatEngine::GameObject> whipArm;
	std::shared_ptr<FlatEngine::GameObject> lineOfSight;

	bool _seesPlayer;
	int chaseTimeout;
	int chaseTimerStarted;
	int maxHealth;
	int attackDamage;

};