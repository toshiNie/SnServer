#include"stdafx.h"
#include"LogFile.h"
#include"Log.h"


NsLog& NsLog::instance()
{
	static NsLog nsLog;
	return nsLog;
}
NsLog::NsLog() :level_(LogLevel::INFO) {}

void NsLog::addLogFile(const std::string& strMoudle, const std::string& strBaseFileName, int size)
{
	if (mapLogFile_.find(strMoudle) == mapLogFile_.end())
	{
		auto ptr = std::make_shared<LogFile>(strBaseFileName, size);
		if (ptr->create())
			mapLogFile_[strMoudle] = ptr;
	}
}

void NsLog::addLogFile(const std::string& strMoudle, const FILE* file)
{
	if (mapLogFile_.find(strMoudle) == mapLogFile_.end())
	{
		auto ptr = std::make_shared<LogFile>(file);
		mapLogFile_[strMoudle] = ptr;

	}
}

void NsLog::addLog(const std::string& strMoudle, const char* data, size_t size)
{
	auto iter = mapLogFile_.find(strMoudle);
	if (iter != mapLogFile_.end())
	{
		std::stringstream ss;
		ss << " [" << strMoudle << "] " << data << "\n";
		iter->second->append(ss.str().c_str(), ss.str().size());
	}
}

void NsLog::addLog(NsLog::LogPackagePtr spLogPackage)
{
	if (spLogPackage->logLevel < level_)
	{
		return;
	}
	auto iter = mapLogFile_.find(spLogPackage->strModule);
	if (iter != mapLogFile_.end())
	{
		std::string  logMessage(spLogPackage->toString());
		iter->second->append(logMessage.c_str(), logMessage.size());
	}
}
void NsLog::flush()
{
	for (auto& iter : mapLogFile_)
	{
		iter.second->flush();
	}
}
