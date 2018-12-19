#pragma once
#include"stdafx.h"
#include"SafeQueue.h"
#include"Log.h"
#include "SigProcess.h"
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
	}
	void put(const std::string &module, const std::string&& log)
	{
		logQueue_.push(std::pair<std::string, std::string>(module, log));
	}
	void addLogFile(const std::string &strMoudle, const std::string& strFileName)
	{
		log_.AddLogFile(strMoudle,strFileName);
	}
	void addLogFile(const std::string &strMoudle, const FILE* flie)
	{
		log_.AddLogFile(strMoudle, flie);
	}
	void run()
	{
		spThread_ = std::shared_ptr<std::thread>(new std::thread(
			[&]() {
			while (true)
			{
				auto pair = std::move(logQueue_.pop());
				log_.AddLog(pair.first, pair.second.c_str(), pair.second.size());
			}
		}
		));
	}
	
	void Flush()
	{
		while (!logQueue_.isEmpty())
		{
			printf("queue size: %d\n", logQueue_.size());
		}
		log_.Flush();
	}

	void join()
	{
		spThread_->join();
	}
private:
	std::shared_ptr<std::thread> spThread_;
	SafeQueue<std::pair<std::string, std::string>> logQueue_;
	NsLog& log_;
};

inline void LOG_INFO(const std::string& strLog)
{
	//printf("%s/n",strLog.c_str());
	std::stringstream ss;
	ss << std::this_thread::get_id() << " " << strLog;
	LogThread::getInstance().put("info", ss.str());
}
