#pragma once
#include"stdafx.h"
#include"LogFile.h"
class NsLog
{
public:
	static NsLog& Instance()
	{
		static NsLog nsLog;
		return nsLog;
	}

	void AddLogFile(const std::string& strMoudle, const std::string& strBaseFileName)
	{
		if (mapLogFile_.find(strMoudle) == mapLogFile_.end())
		{
			auto ptr = LogFilePtr(new LogFile(strBaseFileName));
			if(ptr->Create())
				mapLogFile_[strMoudle] = ptr;

		}
	}

	void AddLog(const std::string& strMoudle,const char* data,size_t size)
	{
		auto iter = mapLogFile_.find(strMoudle);
		if (iter != mapLogFile_.end())
		{
			std::stringstream ss;
			ss << " [" << strMoudle << "] " << data;
			iter->second->Append(ss.str().c_str(),ss.str().size());
		}
	}
private:
	std::map<std::string, LogFilePtr> mapLogFile_;
};

//#define LOG_INFO(A)  NsLog::Instance().AddLog("info",A,strlen(A));
#define LOG_INFO(A)  std::cout<<A<<std::endl;