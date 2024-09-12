#include "TagList.h"
#include "FlatEngine.h"
#include "GameLoop.h"
#include "Scene.h"

namespace FlatEngine 
{
	TagList::TagList()
	{
		m_tags.emplace("Player", false);
		m_tags.emplace("Enemy", false);
		m_tags.emplace("Npc", false);
		m_tags.emplace("Terrain", false);
		m_tags.emplace("PlayerTrigger", false);
		m_tags.emplace("EnemyTrigger", false);
		m_tags.emplace("NpcTrigger", false);
		m_tags.emplace("EnvironmentalTrigger", false);
		m_tags.emplace("TerrainTrigger", false);
		m_tags.emplace("PlayerDamage", false);
		m_tags.emplace("EnemyDamage", false);
		m_tags.emplace("EnvironmentalDamage", false);
		m_tags.emplace("Projectile", false);
		m_tags.emplace("InteractableItem", false);
		m_tags.emplace("InteractableObject", false);
		m_tags.emplace("Item", false);

		m_ignoreTags.emplace("Player", false);
		m_ignoreTags.emplace("Enemy", false);
		m_ignoreTags.emplace("Npc", false);
		m_ignoreTags.emplace("Terrain", false);
		m_ignoreTags.emplace("PlayerTrigger", false);
		m_ignoreTags.emplace("EnemyTrigger", false);
		m_ignoreTags.emplace("NpcTrigger", false);
		m_ignoreTags.emplace("EnvironmentalTrigger", false);
		m_ignoreTags.emplace("TerrainTrigger", false);
		m_ignoreTags.emplace("PlayerDamage", false);
		m_ignoreTags.emplace("EnemyDamage", false);
		m_ignoreTags.emplace("EnvironmentalDamage", false);
		m_ignoreTags.emplace("Projectile", false);
		m_ignoreTags.emplace("InteractableItem", false);
		m_ignoreTags.emplace("InteractableObject", false);
		m_ignoreTags.emplace("Item", false);
	}

	TagList::TagList(TagList* toCopy)
	{
		std::map<std::string, bool>::iterator iterator;
		for (iterator = toCopy->m_tags.begin(); iterator != toCopy->m_tags.end(); iterator++)
		{
			if (m_tags.count(iterator->first) > 0)
				m_tags.at(iterator->first) = iterator->second;
			else
				m_tags.emplace(iterator->first, iterator->second);
		}
	}

	TagList::~TagList()
	{
	}

	void TagList::SetTag(std::string tag, bool b_value, bool b_updateColliderPairs)
	{
		if (m_tags.count(tag) > 0)
		{
			m_tags.at(tag) = b_value;
		}

		if (b_updateColliderPairs)
		{
			GetLoadedScene()->UpdateColliderPairs();
		}
	}

	void TagList::ToggleTag(std::string tag)
	{
		if (m_tags.count(tag) > 0)
		{
			m_tags.at(tag) = !m_tags.at(tag);
		}
		GetLoadedScene()->UpdateColliderPairs();
	}

	bool TagList::HasTag(std::string tag)
	{
		if (m_tags.count(tag) > 0)
		{
			return m_tags.at(tag);
		}
		else
		{
			return false;
		}
	}

	void TagList::SetIgnore(std::string tag, bool b_value, bool b_updateColliderPairs)
	{
		if (m_ignoreTags.count(tag) > 0)
		{
			m_ignoreTags.at(tag) = b_value;
		}

		if (b_updateColliderPairs)
		{
			GetLoadedScene()->UpdateColliderPairs();
		}
	}

	void TagList::ToggleIgnore(std::string tag)
	{
		if (m_ignoreTags.count(tag) > 0)
		{
			m_ignoreTags.at(tag) = !m_ignoreTags.at(tag);
		}
		GetLoadedScene()->UpdateColliderPairs();
	}

	bool TagList::IgnoresTag(std::string tag)
	{
		if (m_ignoreTags.count(tag) > 0)
		{
			return m_ignoreTags.at(tag);
		}
		else 
		{
			return false;
		}
	}

	void TagList::CreateNewTag(std::string tagName, bool b_value)
	{
		m_tags.emplace(tagName, b_value);
	}

	void TagList::RemoveTag(std::string tagName)
	{
		if (m_tags.count(tagName) > 0)
		{
			m_tags.erase(tagName);
		}
	}

	std::map<std::string, bool> TagList::GetTagsMap()
	{
		return m_tags;
	}

	std::map<std::string, bool> TagList::GetIgnoreTagsMap()
	{
		return m_ignoreTags;
	}

	std::vector<std::string> TagList::GetIgnoredTags()
	{
		std::vector<std::string> ignoredTags;
		for (std::pair<std::string, bool> ignoreTag : m_ignoreTags)
		{
			if (ignoreTag.second)
			{
				ignoredTags.push_back(ignoreTag.first);
			}
		}
		return ignoredTags;
	}
}