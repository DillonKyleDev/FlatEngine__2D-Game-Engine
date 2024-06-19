#include "Health.h"
#include "FlatEngine.h"


// After copying the contents of these files to the newly created .h and .cpp files of your new script
// #include the script file in GameLoop.cpp and 
// Add a new if branch for the script name in GameLoop()::Start() function


Health::Health(long ownerID) : GameScript(ownerID)
{
	SetName("Health");
	totalHealth = 100;
	currentHealth = 100;
	_onDeathSet = false;
	_onReviveSet = false;
}

Health::~Health()
{
}

void Health::Start()
{
	FlatEngine::LogString("Health instantiated on: " + GetOwner()->GetName());
}

void Health::Update(float deltaTime)
{

}

int Health::GetCurrentHealth()
{
	return currentHealth;
}

void Health::SetCurrentHealth(int newHealth)
{
	currentHealth = newHealth;
}

int Health::GetTotalHealth()
{
	return totalHealth;
}

void Health::SetTotalHealth(int newTotalHealth)
{
	totalHealth = newTotalHealth;
}

void Health::Damage(int amount)
{
	if (currentHealth - amount > 0)
		currentHealth -= amount;
	else
	{
		currentHealth = 0;
		OnDeath;
	}
}

void Health::Heal(int amount)
{
	currentHealth += amount;
}

void Health::Revive(int healthRecovered)
{
	currentHealth = healthRecovered;
	OnRevive;
}

void Health::SetOnDeath(std::function<void(std::shared_ptr<FlatEngine::GameObject>self, std::shared_ptr<FlatEngine::GameObject>killedBy)> callback)
{
	OnDeath = callback;
	_onDeathSet = true;
}

void Health::SetOnRevive(std::function<void(std::shared_ptr<FlatEngine::GameObject>self)> callback)
{
	OnRevive = callback;
	_onReviveSet = true;
}

bool Health::OnDeathSet()
{
	return _onDeathSet;
}

bool Health::OnReviveSet()
{
	return _onReviveSet;
}

void Health::SetOnDeathSet(bool _set)
{
	_onDeathSet = _set;
}

void Health::SetOnReviveSet(bool _set)
{
	_onReviveSet = _set;
}
