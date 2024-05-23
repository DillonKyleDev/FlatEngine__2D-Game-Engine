#pragma once
#include "FlatEngine.h"

namespace FlatEngine {

	class TagList
	{
	public:
		TagList();
		TagList(const std::shared_ptr<TagList> toCopy);
		~TagList();

		void SetTag(std::string tag, bool _value);
		void ToggleTag(std::string tag);
		bool HasTag(std::string);
		void CreateNewTag(std::string tagName, bool _value = false);
		void RemoveTag(std::string tagName);
		std::map<std::string, bool> GetTagsMap();
		
	private:
		std::map<std::string, bool> tags;
	};

}




