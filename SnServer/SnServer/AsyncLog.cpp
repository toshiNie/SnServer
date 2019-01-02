#include"stdafx.h"
#include"AsyncLog.h"

AsyncLog::AsyncLog() :log_(NsLog::instance()),isCancel_(false)
{

}

AsyncLog& AsyncLog::getInstance()
{
	static AsyncLog asyncLog;
	return asyncLog;
}

AsyncLog::~AsyncLog()
{

}

void AsyncLog::put(NsLog::LogPackagePtr  upLogPackage)
{
	if (!isCancel_)
	{
		logQueue_.push(std::move(upLogPackage));
	}
}

void AsyncLog::addLogFile(const std::string &strMoudle, const std::string& strFileName)
{
	log_.addLogFile(strMoudle, strFileName);
}

void AsyncLog::addLogFile(const std::string &strMoudle, const FILE* flie)
{
	log_.addLogFile(strMoudle, flie);
}

void AsyncLog::run()
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

void AsyncLog::Flush()
{
	while (!logQueue_.isEmpty())
	{
	}
	log_.flush();
}

void AsyncLog::join()
{
	spThread_->join();
}