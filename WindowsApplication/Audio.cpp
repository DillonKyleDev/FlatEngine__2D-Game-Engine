#include "Audio.h"
#include "FlatEngine.h"

namespace FlatEngine
{
	Audio::Audio(long myID, long parentID)
	{
		this->SetType(ComponentTypes::Audio);
		this->SetID(myID);
		this->SetParentID(parentID);
		this->sound = std::make_shared<Sound>();
		this->_isMusic = false;
	}

	Audio::Audio(std::shared_ptr<Audio> toCopy, long newParentID)
	{
		this->SetType(ComponentTypes::Audio);
		this->SetID(GetNextComponentID());
		this->SetParentID(newParentID);
		this->_isMusic = toCopy->IsMusic();
		this->sound = toCopy->sound;
	}

	Audio::~Audio()
	{
	}

	void Audio::LoadMusic(std::string path)
	{
		this->sound->LoadMusic(path);
	}

	void Audio::LoadEffect(std::string path)
	{
		this->sound->LoadEffect(path);
	}

	void Audio::SetPath(std::string path)
	{
		this->path = path;
	}

	std::string Audio::GetPath()
	{
		return this->path;
	}

	void Audio::SetIsMusic(bool _isMusic)
	{
		this->_isMusic = _isMusic;
	}

	bool Audio::IsMusic()
	{
		return this->_isMusic;
	}

	void Audio::Play(int channel)
	{
		if (this->_isMusic)
			this->sound->PlayMusic();
		else
			this->sound->PlayEffect(channel);
	}

	void Audio::Pause(int channel)
	{
		if (this->_isMusic)
			this->sound->PauseMusic();
		else
			this->sound->HaultChannel(channel);
	}

	void Audio::Stop(int channel)
	{
		if (this->_isMusic)
			this->sound->StopMusic();
		else
			this->sound->HaultChannel(channel);
	}

	bool Audio::IsMusicPlaying()
	{
		return this->sound->IsMusicPlaying();
	}

	std::string Audio::GetData()
	{
		json jsonData = {
			{ "type", "Audio" },
			{ "id", this->GetID() },
			{ "_isCollapsed", this->IsCollapsed() },
			{ "path", this->path },
			{ "_isMusic", this->_isMusic },
		};

		std::string data = jsonData.dump();
		// Return dumped json object with required data for saving
		return data;
	}
}