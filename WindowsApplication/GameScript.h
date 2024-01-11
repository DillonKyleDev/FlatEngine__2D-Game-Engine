#pragma once
#include "GameObject.h"
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
		// Entities will be added at game run time for now. Could move this to 
		// right as the script string gets attached to the entity later.
		void AddEntity(std::shared_ptr<GameObject> entity);
		std::vector<std::shared_ptr<GameObject>> GetEntities();

		bool _isActive;

	private:
		std::vector<std::shared_ptr<GameObject>> entities;
	};
}