#pragma once
#ifndef GAME_OBJECT
#define GAME_OBJECT
#include "GameObject.h"
#endif
#include <string>
#include <vector>
#include <memory>


namespace FlatEngine
{
	class GameScript
	{
	public:
		GameScript();
		~GameScript();

		virtual void Awake() {}
		virtual void Start() {}
		virtual void Update(float deltaTime) {}
		void SetName(std::string name);
		std::string GetName();
		// Entities will be added at game run time for now. Could move this to 
		// right as the script string gets attached to the entity later.
		void AddEntity(std::shared_ptr<GameObject> entity);
		std::vector<std::shared_ptr<GameObject>> GetEntities();
		void SetOwner(std::shared_ptr<GameObject> owner);
		std::shared_ptr<GameObject> GetOwner();

		bool _isActive;

	private:
		std::string name;
		std::vector<std::shared_ptr<GameObject>> entities;
		std::shared_ptr<GameObject> owner;
	};
}