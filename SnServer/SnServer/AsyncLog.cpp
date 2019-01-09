#include"stdafx.h"
#include"LogFile.h"
#include"AsyncLog.h"


AsyncLog::AsyncLog():isCancel_(false),level_(LogLevel::INFO),rollSize_(1024*1024*100)
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

void AsyncLog::put(const char* moudle, std::string&& strlog)
{
	if (!isCancel_)
	{
		auto iter = logThreads_.find(moudle);
		if(iter != logThreads_.end())
		{ 
			iter->second->queue.push(strlog);
		}
	}
}

void AsyncLog::addLogFile(const std::string &strMoudle, const std::string& strFileName)
{
	logThreads_[strMoudle] = std::make_shared<LogThread>(strFileName, rollSize_);
}

void AsyncLog::addLogFile(const std::string &strMoudle, FILE* flie)
{
	logThreads_[strMoudle] = std::make_shared<LogThread>(flie);
}

void AsyncLog::run()
{
	for (auto &item : logThreads_)
	{
		threads_.emplace_back(ThreadRAII(std::thread(std::bind(&LogThread::run, item.second)), ThreadRAII::DtorAction::join));
	}
}

void AsyncLog::flush()
{
	while (true) {
		bool ret = true;
		for (auto & item : logThreads_)
		{
			ret = ret && item.second->queue.isEmptySafe();
		}
		if (ret)
		{
			break;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
	for (auto & item : logThreads_)
	{
		item.second->spLogFile->flush();
	}
}
