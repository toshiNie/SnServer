#include"stdafx.h"
#include"LogFile.h"
#include"Log.h"


Log& Log::instance()
{
	static Log nsLog;
	return nsLog;
}
Log::Log() :level_(LogLevel::INFO) {}

void Log::addLogFile(const std::string& strMoudle, const std::string& strBaseFileName, int size)
{
	if (mapLogFile_.find(strMoudle) == mapLogFile_.end())
	{
		auto ptr = std::make_shared<LogFile>(strBaseFileName, size);
		if (ptr->create())
			mapLogFile_[strMoudle] = ptr;
	}
}

void Log::addLogFile(const std::string& strMoudle, const FILE* file)
{
	if (mapLogFile_.find(strMoudle) == mapLogFile_.end())
	{
		auto ptr = std::make_shared<LogFile>(file);
		mapLogFile_[strMoudle] = ptr;

	}
}

void Log::addLog(const std::string& strMoudle, const char* data, size_t size)
{
	auto iter = mapLogFile_.find(strMoudle);
	if (iter != mapLogFile_.end())
	{
		std::stringstream ss;
		ss << " [" << strMoudle << "] " << data << "\n";
		iter->second->append(ss.str().c_str(), ss.str().size());
	}
}

void Log::addLog(Log::LogPackagePtr spLogPackage)
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
void Log::flush()
{
	for (auto& iter : mapLogFile_)
	{
		iter.second->flush();
	}
}
