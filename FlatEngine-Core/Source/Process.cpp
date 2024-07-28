#include "Process.h"

namespace FlatEngine {
	Process::Process(std::string name)
	{
		processName = name;
		rawHangTimeData = std::deque<float>();
		tickCounter = 0;
	}

	Process::~Process()
	{
	}

	std::string Process::GetProcessName()
	{
		return processName;
	}

	void Process::AddHangTimeData(float hangTime)
	{
		if (rawHangTimeData.size() < 100)
		{
			rawHangTimeData.push_front(hangTime);
		}
		else if (rawHangTimeData.size() >= 100)
		{
			rawHangTimeData.pop_front();
			rawHangTimeData.push_back(hangTime);
		}


		//if (tickCounter < 100)
		//{
		//	std::vector<float>::iterator iter = snapShotData.begin();

		//	if (snapShotData.size() >= 100)
		//		snapShotData.insert(iter + tickCounter, rawHangTimeData[tickCounter]);
		//	else
		//		snapShotData.push_back(rawHangTimeData[tickCounter]);

		//	tickCounter++;
		//}
		//else if (tickCounter >= 100 && tickCounter < 200)
		//	tickCounter++;
		//else if (tickCounter == 200)
		//{
		//	tickCounter = 0;
		//}


		if (tickCounter < 100)
		{
			tickCounter++;
		}
		else if (tickCounter == 100)
		{
			snapShotData.clear();
			for (int i = 0; i < 100; i++)
				snapShotData.push_back(rawHangTimeData[i]);
			tickCounter++;
		}
		else if (tickCounter > 100 && tickCounter < 200)
			tickCounter++;
		else if (tickCounter == 200)
		{
			tickCounter = 0;
		}

	}

	std::deque<float> Process::GetRawData()
	{
		return rawHangTimeData;
	}

	std::vector<float> Process::GetHangTimeData()
	{
		return snapShotData;
	}
}