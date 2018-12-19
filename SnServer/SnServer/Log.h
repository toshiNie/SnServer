#pragma once
#ifndef NSLOG_H_
#define NSLOG_H_
#include"stdafx.h"
#include"LogFile.h"
class NsLog
{
public:
	static NsLog& Instance();

	void AddLogFile(const std::string& strMoudle, const std::string& strBaseFileName);

	void AddLogFile(const std::string& strMoudle, const FILE* file);

	void AddLog(const std::string& strMoudle, const char* data, size_t size);

	void Flush();
private:
	std::map<std::string, LogFilePtr> mapLogFile_;
	std::mutex mutex_;
};

//#define LOG_INFO(A)  NsLog::Instance().AddLog("info",A,strlen(A));
//#define LOG_INFO(A)  std::cout<<A<<std::endl;

#endif