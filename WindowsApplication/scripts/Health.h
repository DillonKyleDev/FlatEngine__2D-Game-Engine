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
	void Damage(std::shared_ptr<FlatEngine::GameObject> damagedBy, int amount);
	void Heal(std::shared_ptr<FlatEngine::GameObject> healedBy, int amount);
	void Revive(std::shared_ptr<FlatEngine::GameObject> revivedBy, int healthRecovered);

	void SetOnTakeDamage(std::function<void(std::shared_ptr<FlatEngine::GameObject> self, std::shared_ptr<FlatEngine::GameObject> damagedBy, float damageAmount)> callback);
	void SetOnDeath(std::function<void(std::shared_ptr<FlatEngine::GameObject> self, std::shared_ptr<FlatEngine::GameObject> killedBy)> callback);
	void SetOnRevive(std::function<void(std::shared_ptr<FlatEngine::GameObject> self, std::shared_ptr<FlatEngine::GameObject> revivedBy, int healthRecovered)> callback);
	std::function<void(std::shared_ptr<FlatEngine::GameObject> self, std::shared_ptr<FlatEngine::GameObject> damagedBy, float damageAmount)> OnTakeDamage;
	std::function<void(std::shared_ptr<FlatEngine::GameObject> self, std::shared_ptr<FlatEngine::GameObject> killedBy)> OnDeath;
	std::function<void(std::shared_ptr<FlatEngine::GameObject> self, std::shared_ptr<FlatEngine::GameObject> revivedBy, int healthRecovered)> OnRevive;
	bool OnDeathSet();
	bool OnReviveSet();
	void SetOnTakeDamageSet(bool _set);
	void SetOnDeathSet(bool _set);
	void SetOnReviveSet(bool _set);

private:
	int totalHealth;
	int currentHealth;
	bool _onTakeDamageSet;
	bool _onDeathSet;
	bool _onReviveSet;
};