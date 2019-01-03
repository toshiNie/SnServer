#include"stdafx.h"
#include"Log.h"


NsLog& NsLog::instance()
{
	static NsLog nsLog;
	return nsLog;
}
NsLog::NsLog() :level_(INFO) {}

void NsLog::addLogFile(const std::string& strMoudle, const std::string& strBaseFileName, int size)
{
	if (mapLogFile_.find(strMoudle) == mapLogFile_.end())
	{
		auto ptr = LogFilePtr(new LogFile(strBaseFileName, size));
		if (ptr->Create())
			mapLogFile_[strMoudle] = ptr;

	}
}


void NsLog::addLogFile(const std::string& strMoudle, const FILE* file)
{
	if (mapLogFile_.find(strMoudle) == mapLogFile_.end())
	{
		auto ptr = LogFilePtr(new LogFile(file));
		mapLogFile_[strMoudle] = ptr;

	}
}

void NsLog::addLog(const std::string& strMoudle, const char* data, size_t size)
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

void NsLog::addLog(NsLog::LogPackagePtr spLogPackage)
{
	if (spLogPackage->logLevel < level_)
	{
		return;
	}
	//std::lock_guard<std::mutex> lg(mutex_);
	auto iter = mapLogFile_.find(spLogPackage->strModule);
	if (iter != mapLogFile_.end())
	{
		std::string  logMessage(spLogPackage->toString());
		iter->second->Append(logMessage.c_str(), logMessage.size());
	}
}
void NsLog::flush()
{
	for (auto it = mapLogFile_.begin(); it != mapLogFile_.end(); it++)
	{
		it->second->Flush();
	}
}
