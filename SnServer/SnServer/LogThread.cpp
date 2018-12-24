#include"stdafx.h"
#include"LogThread.h"

LogThread::LogThread() :log_(NsLog::instance()),isCancel_(false)
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

void LogThread::put(NsLog::LogPackagePtr  upLogPackage)
{
	if (!isCancel_)
	{
		logQueue_.push(std::move(upLogPackage));
	}
}

void LogThread::addLogFile(const std::string &strMoudle, const std::string& strFileName)
{
	log_.addLogFile(strMoudle, strFileName);
}

void LogThread::addLogFile(const std::string &strMoudle, const FILE* flie)
{
	log_.addLogFile(strMoudle, flie);
}

void LogThread::run()
{
	spThread_ = std::shared_ptr<std::thread>(new std::thread(
		[&]() {
		while (!isCancel_)
		{
			auto spMessage = std::move(logQueue_.pop());
			log_.addLog(spMessage);
		}
	}
	));
}

void LogThread::Flush()
{
	while (!logQueue_.isEmpty())
	{
	}
	log_.flush();
}

void LogThread::join()
{
	spThread_->join();
}