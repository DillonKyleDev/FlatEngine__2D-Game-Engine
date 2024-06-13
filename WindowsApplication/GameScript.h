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
		GameScript(long ownerID = -1);
		~GameScript();

		virtual void Awake() {}
		virtual void Start() {}
		virtual void Update(float deltaTime) {}
		void SetName(std::string name);
		std::string GetName();
		long GetOwnerID();
		void SetOwnerID(long id);
		void SetOwner(std::shared_ptr<GameObject> owner);
		std::shared_ptr<GameObject> GetOwner();

		bool _isActive;

	private:
		std::string name;
		// Actually the ID of the ScriptComponent, not the GameObject that owns the ScriptComponent
		long ownerID;
		std::shared_ptr<GameObject> owner;
	};
}