#pragma once
#include "GameScript.h"
#include <functional>


class Health : public FlatEngine::GameScript
{
public:
	Health(long ownerID);
	~Health();

	void Start();
	void Update(float deltaTime);
	int GetCurrentHealth();
	void SetCurrentHealth(int newHealth);
	int GetTotalHealth();
	void SetTotalHealth(int newTotalHealth);
	void Damage(int amount);
	void Heal(int amount);
	void Revive(int healthRecovered);

	void SetOnDeath(std::function<void(std::shared_ptr<FlatEngine::GameObject> self, std::shared_ptr<FlatEngine::GameObject> killedBy)> callback);
	void SetOnRevive(std::function<void(std::shared_ptr<FlatEngine::GameObject> self)> callback);
	std::function<void(std::shared_ptr<FlatEngine::GameObject> self, std::shared_ptr<FlatEngine::GameObject> killedBy)> OnDeath;
	std::function<void(std::shared_ptr<FlatEngine::GameObject> self)> OnRevive;
	bool OnDeathSet();
	bool OnReviveSet();
	void SetOnDeathSet(bool _set);
	void SetOnReviveSet(bool _set);

private:
	int totalHealth;
	int currentHealth;
	bool _onDeathSet;
	bool _onReviveSet;
};