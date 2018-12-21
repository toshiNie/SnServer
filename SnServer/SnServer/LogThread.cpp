#include"stdafx.h"
#include"LogThread.h"

LogThread::LogThread() :log_(NsLog::Instance()),isCancel_(false)
{

}

LogThread& LogThread::getInstance()
{
	static LogThread logThread;
	return logThread;
}

LogThread::~LogThread()
{

}

void LogThread::put(const std::string &module, const std::string&& log)
{
	logQueue_.push(std::pair<std::string, std::string>(module, log));
}

void LogThread::addLogFile(const std::string &strMoudle, const std::string& strFileName)
{
	log_.AddLogFile(strMoudle, strFileName);
}

void LogThread::addLogFile(const std::string &strMoudle, const FILE* flie)
{
	log_.AddLogFile(strMoudle, flie);
}

void LogThread::run()
{
	spThread_ = std::shared_ptr<std::thread>(new std::thread(
		[&]() {
		while (!isCancel_)
		{
			auto pair = std::move(logQueue_.pop());
			log_.AddLog(pair.first, pair.second.c_str(), pair.second.size());
		}
		std::cout << "exit" << std::endl;
	}
	));
}

void LogThread::Flush()
{
	while (!logQueue_.isEmpty())
	{
	}
	log_.Flush();
}

void LogThread::join()
{
	spThread_->join();
}