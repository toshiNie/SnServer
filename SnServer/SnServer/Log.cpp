#include"stdafx.h"
#include"Log.h"


NsLog& NsLog::Instance()
{
	static NsLog nsLog;
	return nsLog;
}

void NsLog::AddLogFile(const std::string& strMoudle, const std::string& strBaseFileName)
{
	if (mapLogFile_.find(strMoudle) == mapLogFile_.end())
	{
		auto ptr = LogFilePtr(new LogFile(strBaseFileName));
		if (ptr->Create())
			mapLogFile_[strMoudle] = ptr;

	}
}


void NsLog::AddLogFile(const std::string& strMoudle, const FILE* file)
{
	if (mapLogFile_.find(strMoudle) == mapLogFile_.end())
	{
		auto ptr = LogFilePtr(new LogFile(file));
		if (ptr->Create())
			mapLogFile_[strMoudle] = ptr;

	}
}

void NsLog::AddLog(const std::string& strMoudle, const char* data, size_t size)
{
	//std::lock_guard<std::mutex> lg(mutex_);
	auto iter = mapLogFile_.find(strMoudle);
	if (iter != mapLogFile_.end())
	{
		std::stringstream ss;
		ss << " [" << strMoudle << "] " << data << "\n";
		iter->second->Append(ss.str().c_str(), ss.str().size());
	}
}

void NsLog::Flush()
{
	for (auto it = mapLogFile_.begin(); it != mapLogFile_.end(); it++)
	{
		it->second->Flush();
	}
}
