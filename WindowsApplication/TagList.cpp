#include "TagList.h"
#include "FlatEngine.h"


namespace FlatEngine {

	TagList::TagList()
	{
		tags.emplace("Player", false);
		tags.emplace("Enemy", false);
		tags.emplace("Npc", false);
		tags.emplace("Terrain", false);
		tags.emplace("PlayerTrigger", false);
		tags.emplace("EnemyTrigger", false);
		tags.emplace("NpcTrigger", false);
		tags.emplace("EnvironmentalTrigger", false);
		tags.emplace("TerrainTrigger", false);
		tags.emplace("PlayerDamage", false);
		tags.emplace("EnemyDamage", false);
		tags.emplace("EnvironmentalDamage", false);
		tags.emplace("Projectile", false);
		tags.emplace("InteractableItem", false);
		tags.emplace("InteractableObject", false);
		tags.emplace("Item", false);

		ignoreTags.emplace("Player", false);
		ignoreTags.emplace("Enemy", false);
		ignoreTags.emplace("Npc", false);
		ignoreTags.emplace("Terrain", false);
		ignoreTags.emplace("PlayerTrigger", false);
		ignoreTags.emplace("EnemyTrigger", false);
		ignoreTags.emplace("NpcTrigger", false);
		ignoreTags.emplace("EnvironmentalTrigger", false);
		ignoreTags.emplace("TerrainTrigger", false);
		ignoreTags.emplace("PlayerDamage", false);
		ignoreTags.emplace("EnemyDamage", false);
		ignoreTags.emplace("EnvironmentalDamage", false);
		ignoreTags.emplace("Projectile", false);
		ignoreTags.emplace("InteractableItem", false);
		ignoreTags.emplace("InteractableObject", false);
		ignoreTags.emplace("Item", false);
	}

	TagList::TagList(const std::shared_ptr<TagList> toCopy)
	{
		std::map<std::string, bool>::iterator iterator;
		for (iterator = toCopy->tags.begin(); iterator != toCopy->tags.end(); iterator++)
		{
			if (tags.count(iterator->first) > 0)
				tags.at(iterator->first) = iterator->second;
			else
				tags.emplace(iterator->first, iterator->second);
		}
	}

	TagList::~TagList()
	{
	}

	void TagList::SetTag(std::string tag, bool _value)
	{
		if (tags.count(tag) > 0)
			tags.at(tag) = _value;
		FlatEngine::gameLoop->UpdateActiveColliders();
	}

	void TagList::ToggleTag(std::string tag)
	{
		if (tags.count(tag) > 0)
			tags.at(tag) = !tags.at(tag);
		FlatEngine::gameLoop->UpdateActiveColliders();
	}

	bool TagList::HasTag(std::string tag)
	{
		if (tags.count(tag) > 0)
			return tags.at(tag);
		else return false;
	}

	void TagList::SetIgnore(std::string tag, bool _value)
	{
		if (ignoreTags.count(tag) > 0)
			ignoreTags.at(tag) = _value;
		FlatEngine::gameLoop->UpdateActiveColliders();
	}

	void TagList::ToggleIgnore(std::string tag)
	{
		if (ignoreTags.count(tag) > 0)
			ignoreTags.at(tag) = !ignoreTags.at(tag);
		FlatEngine::gameLoop->UpdateActiveColliders();
	}

	bool TagList::IgnoresTag(std::string tag)
	{
		if (ignoreTags.count(tag) > 0)
			return ignoreTags.at(tag);
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

	std::map<std::string, bool> TagList::GetIgnoreTagsMap()
	{
		return ignoreTags;
	}

	std::vector<std::string> TagList::GetIgnoredTags()
	{
		std::vector<std::string> ignoredTags;
		for (std::pair<std::string, bool> ignoreTag : ignoreTags)
		{
			if (ignoreTag.second)
				ignoredTags.push_back(ignoreTag.first);
		}
		return ignoredTags;
	}
}