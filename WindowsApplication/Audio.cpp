#include "Audio.h"
#include "FlatEngine.h"

namespace FlatEngine
{
	Audio::Audio(long myID, long parentID)
	{
		SetType(ComponentTypes::Audio);
		SetID(myID);
		SetParentID(parentID);
		sound = std::make_shared<Sound>();
		_isMusic = false;
	}

	Audio::Audio(std::shared_ptr<Audio> toCopy, long newParentID)
	{
		SetType(ComponentTypes::Audio);
		SetID(GetNextComponentID());
		SetParentID(newParentID);
		SetActive(toCopy->IsActive());
		_isMusic = toCopy->IsMusic();
		sound = toCopy->sound;
		path = toCopy->path;
	}

	Audio::~Audio()
	{
	}

	void Audio::LoadMusic(std::string path)
	{
		sound->LoadMusic(path);
	}

	void Audio::LoadEffect(std::string path)
	{
		sound->LoadEffect(path);
	}

	void Audio::SetPath(std::string newPath)
	{
		path = newPath;
		LoadEffect(newPath);
	}

	std::string Audio::GetPath()
	{
		return path;
	}

	void Audio::SetIsMusic(bool _music)
	{
		_isMusic = _music;
	}

	bool Audio::IsMusic()
	{
		return _isMusic;
	}

	void Audio::Play(int channel)
	{
		if (_isMusic)
			sound->PlayMusic();
		else
			sound->PlayEffect(channel);
	}

	void Audio::Pause(int channel)
	{
		if (_isMusic)
			sound->PauseMusic();
		else
			sound->HaultChannel(channel);
	}

	void Audio::Stop(int channel)
	{
		if (_isMusic)
			sound->StopMusic();
		else
			sound->HaultChannel(channel);
	}

	bool Audio::IsMusicPlaying()
	{
		return sound->IsMusicPlaying();
	}

	std::string Audio::GetData()
	{
		json jsonData = {
			{ "type", "Audio" },
			{ "id", GetID() },
			{ "_isCollapsed", IsCollapsed() },
			{ "_isActive", IsActive() },
			{ "path", path },
			{ "_isMusic", _isMusic },
		};

		std::string data = jsonData.dump();
		// Return dumped json object with required data for saving
		return data;
	}
}