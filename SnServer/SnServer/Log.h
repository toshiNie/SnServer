#pragma once
#ifndef NSLOG_H_
#define NSLOG_H_
#include"stdafx.h"
class LogFile;
class NsLog
{
public:
	enum class LogLevel
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
		LogLevel logLevel;
		std::string timeStamp;
		std::string logMessage;
		std::string threadId;

		const char * levelToString(LogLevel level)
		{
			switch (level)
			{
			case LogLevel::DEBUG:
				return "[DEBUG]";
			case LogLevel::INFO:
				return "[INFO]";
			case LogLevel::WARNING:
				return "[WARNING]";
			case LogLevel::ERROR:
				return "[ERROR]";
			case LogLevel::FATEL:
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
	NsLog();
	static NsLog& instance();
	void addLogFile(const std::string& strMoudle, const std::string& strBaseFileName , int size = 100*1024*1024);
	void addLogFile(const std::string& strMoudle, const FILE* file);
	void addLog(const std::string& strMoudle, const char* data, size_t size);
	void addLog(LogPackagePtr spLogPackage);
	void flush();
	void setLevel(LogLevel level)
	{
		level_ = level;
	}
	LogLevel getLevel()
	{
		return level_;
	}
private:
	std::map<std::string, std::shared_ptr<LogFile> > mapLogFile_;
	LogLevel level_;
	std::mutex mutex_;
};


#endif