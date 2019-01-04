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
	spThread_ = std::make_unique<ThreadRAII>(ThreadRAII(std::thread(
		[&]() {
		while (!isCancel_)
		{
			auto spMessage = std::move(logQueue_.pop());
			log_.addLog(spMessage);
		}
	}
	), ThreadRAII::DtorAction::join));
}

void AsyncLog::flush()
{
	while (!logQueue_.isEmptySafe())
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
	log_.flush();
}
