#pragma once
#include "FlatEngine.h"

namespace FlatEngine {

	class TagList
	{
	public:
		TagList();
		TagList(const TagList* toCopy);
		~TagList();

		void SetTag(std::string tag, bool _value);
		bool HasTag(std::string);
		void CreateNewTag(std::string tagName, bool _value = false);
		void RemoveTag(std::string tagName);
		std::map<std::string, bool> GetTagsMap();
		
	private:
		std::map<std::string, bool> tags;
		bool _player;
		bool _enemy;
		bool _npc;
		bool _onlyForPlayer;
		bool _onlyForEnemy;
		bool _projectile;
		bool _terrain;
		bool _interactableItem;
		bool _interactableObject;
		bool _item;
	};

}




