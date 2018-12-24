#pragma once
#include"stdafx.h"
#include"SafeQueue.h"
#include"Log.h"
#include "SigProcess.h"
class LogThread
{
public:
	LogThread();

	static LogThread& getInstance();

	~LogThread();

	void put(NsLog::LogPackagePtr  upLogPackage);

	void addLogFile(const std::string &strMoudle, const std::string& strFileName);

	void addLogFile(const std::string &strMoudle, const FILE* flie);

	void run();

	void Flush();

	void join();
	void setCancel()
	{
		isCancel_ = true;

	}
private:
	std::shared_ptr<std::thread> spThread_;
	SafeQueue<NsLog::LogPackagePtr> logQueue_;
	bool isCancel_;
	NsLog& log_;
};

template<typename T>
void LOG(T& log, NsLog::LOG_LEVEL level)
{
	if (level < NsLog::instance().getLevel())
	{
		return;
	}
	NsLog::LogPackagePtr spLogpackage(new NsLog::LogPackage);
	std::stringstream ss;
	ss << std::this_thread::get_id();
	spLogpackage->threadId = ss.str();
	spLogpackage->logLevel = level;
	spLogpackage->strModule = "info";
	spLogpackage->timeStamp = NsTime::GetStrTimeStamp();
	spLogpackage->logMessage = log;
	LogThread::getInstance().put(spLogpackage);
}

inline void LOG_INFO(const std::string& strLog)
{
	//return;
	LOG(strLog, NsLog::INFO);
}

inline void LOG_DEBUG(const std::string& strLog)
{
	//return;
	LOG(strLog, NsLog::DEBUG);
}

inline void LOG_ERROR(const std::string& strLog)
{
	//return;
	LOG(strLog, NsLog::ERROR);
}

