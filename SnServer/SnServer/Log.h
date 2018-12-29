#pragma once
#ifndef NSLOG_H_
#define NSLOG_H_
#include"stdafx.h"
#include"LogFile.h"
class NsLog
{
public:
	enum LOG_LEVEL
	{
		DEBUG,
		INFO,
		WARNING,
		ERROR,
		FATEL
	};
	struct LogPackage
	{
		std::string strModule;
		LOG_LEVEL logLevel;
		std::string timeStamp;
		std::string logMessage;
		std::string threadId;

		const char * levelToString(LOG_LEVEL level)
		{
			switch (level)
			{
			case DEBUG:
				return "[DEBUG]";
			case INFO:
				return "[INFO]";
			case WARNING:
				return "[WARNING]";
			case ERROR:
				return "[ERROR]";
			case FATEL:
				return "[FATEL]";
			default:
				return "[UNKNOWEN]";
			}

		}
		std::string toString()
		{
			std::stringstream ss;
			ss << timeStamp << " "
				<< levelToString(logLevel) << " "
				<< threadId << " "
				<< logMessage << "\n";
			return ss.str();
		}
	};
	typedef std::shared_ptr<LogPackage> LogPackagePtr;
public:
	NsLog() :level_(INFO) {}
	static NsLog& instance();
	void addLogFile(const std::string& strMoudle, const std::string& strBaseFileName , int size = 100*1024*1024);
	void addLogFile(const std::string& strMoudle, const FILE* file);
	void addLog(const std::string& strMoudle, const char* data, size_t size);
	void addLog(LogPackagePtr spLogPackage);
	void flush();
	void setLevel(LOG_LEVEL level)
	{
		level_ = level;
	}
	LOG_LEVEL getLevel()
	{
		return level_;
	}
private:
	std::map<std::string, LogFilePtr> mapLogFile_;
	LOG_LEVEL level_;
	std::mutex mutex_;
};

//#define LOG_INFO(A)  NsLog::Instance().AddLog("info",A,strlen(A));
//#define LOG_INFO(A)  std::cout<<A<<std::endl;

#endif