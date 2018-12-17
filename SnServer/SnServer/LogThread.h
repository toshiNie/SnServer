#pragma once
#include"stdafx.h"
#include"SafeQueue.h"
#include"Log.h"
class LogThread
{
public:
	LogThread():log_(NsLog::Instance()) 
	{
	}
	static LogThread& getInstance()
	{
		static LogThread logThread;
		return logThread;
	}
	~LogThread()
	{
		spThread_->join();
	}
	void put(const std::string &module, const std::string& log)
	{
		logQueue_.push(std::pair<std::string, std::string>(module, log));
	}
	void addLogFile(const std::string &strMoudle, const std::string& strFileName)
	{
		log_.AddLogFile(strMoudle,strFileName);
	}
	void run()
	{
		spThread_ = std::shared_ptr<std::thread>(new std::thread(
			[&]() {
			while (true)
			{
				auto pair = logQueue_.pop();
				log_.AddLog(pair.first, pair.second.c_str(), pair.second.size());
			}
		}
		));
	}
private:
	std::shared_ptr<std::thread> spThread_;
	SafeQueue<std::pair<std::string, std::string>> logQueue_;
	NsLog& log_;
};

inline void LOG_INFO(const std::string& strLog)
{
	LogThread::getInstance().put("info", strLog);
}