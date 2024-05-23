#include "TagList.h"


namespace FlatEngine {

	TagList::TagList()
	{
		tags.emplace("Player", false);
		tags.emplace("Enemy", false);
		tags.emplace("Npc", false);
		tags.emplace("OnlyForPlayer", false);
		tags.emplace("OnlyForEnemy", false);
		tags.emplace("Projectile", false);
		tags.emplace("Terrain", false);
		tags.emplace("InteractableItem", false);
		tags.emplace("InteractableObject", false);
		tags.emplace("Item", false);
	}

	TagList::TagList(const TagList* toCopy)
	{
	}

	TagList::~TagList()
	{
	}

	void TagList::SetTag(std::string tag, bool _value)
	{
		if (tags.count(tag) > 0)
			tags.at(tag) = _value;
	}

	bool TagList::HasTag(std::string tag)
	{
		if (tags.count(tag) > 0)
			return tags.at(tag);
		else return false;
	}

	void TagList::CreateNewTag(std::string tagName, bool _value)
	{
		tags.emplace(tagName, _value);
	}

	void TagList::RemoveTag(std::string tagName)
	{
		if (tags.count(tagName) > 0)
			tags.erase(tagName);
	}

	std::map<std::string, bool> TagList::GetTagsMap()
	{
		return tags;
	}

}