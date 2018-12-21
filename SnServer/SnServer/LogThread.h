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

	void put(const std::string &module, const std::string&& log);

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
	SafeQueue<std::pair<std::string, std::string>> logQueue_;
	bool isCancel_;
	NsLog& log_;
};

inline void LOG_INFO(const std::string& strLog)
{
	//printf("%s/n",strLog.c_str());
	std::stringstream ss;
	ss << std::this_thread::get_id() << " " << strLog;
	LogThread::getInstance().put("info", ss.str());
}
