#include "Logger.h"



namespace FlatEngine
{
	Logger::Logger()
	{
		this->log = new ImGuiTextBuffer();
	}

	Logger::~Logger()
	{

	}

	void Logger::Log(std::string line)
	{
		line = line + "\n";
		this->log->appendf(line.c_str());
	}

	ImGuiTextBuffer *Logger::GetBuffer()
	{
		return this->log;
	}

	void Logger::ClearBuffer()
	{
		this->log->clear();
	}
}