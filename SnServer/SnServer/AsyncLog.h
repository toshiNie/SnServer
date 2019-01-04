#pragma once
#include"stdafx.h"
#include"SafeQueue.h"
#include"Log.h"
#include"TreadRAII.h"
#include "noncopyable.h"
#include "DoubleBufferedQueue.h"
class AsyncLog : public noncopyable
{
public:
	AsyncLog();

	static AsyncLog& getInstance();

	~AsyncLog();

	void put(NsLog::LogPackagePtr  upLogPackage);

	void addLogFile(const std::string &strMoudle, const std::string& strFileName);

	void addLogFile(const std::string &strMoudle, const FILE* flie);

	void run();
	 
	void flush();

	void setCancel()
	{
		isCancel_ = true;
	}
private:
	std::unique_ptr<ThreadRAII> spThread_;
	//SafeQueue<NsLog::LogPackagePtr> logQueue_;
	DoubleBufferedQueue<NsLog::LogPackagePtr> logQueue_;
	bool isCancel_;
	NsLog& log_;
};

template<typename T>
void LOG(T& log, NsLog::LogLevel level)
{
	if (level < NsLog::instance().getLevel())
	{
		return;
	}
	auto spLogpackage = std::make_shared<NsLog::LogPackage>();
	std::stringstream ss;
	ss << std::this_thread::get_id();
	spLogpackage->threadId = ss.str();
	spLogpackage->logLevel = level;
	spLogpackage->strModule = "info";
	spLogpackage->timeStamp = NsTime::GetStrTimeStamp();
	spLogpackage->logMessage = log;
	AsyncLog::getInstance().put(spLogpackage);
}

inline void LOG_INFO(const std::string& strLog)
{
	//return;
	LOG(strLog, NsLog::LogLevel::INFO);
}

inline void LOG_DEBUG(const std::string& strLog)
{
	//return;
	LOG(strLog, NsLog::LogLevel::DEBUG);
}

inline void LOG_ERROR(const std::string& strLog)
{
	//return;
	LOG(strLog, NsLog::LogLevel::ERROR);
}

