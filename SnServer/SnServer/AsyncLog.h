#pragma once
#include"stdafx.h"
#include"SafeQueue.h"
#include"LogFile.h"
#include"TreadRAII.h"
#include "noncopyable.h"
#include "DoubleBufferedQueue.h"

enum class LogLevel
{
	DEBUG,
	INFO,
	WARNING,
	ERROR,
	FATEL
};
class AsyncLog : public noncopyable
{
public:
	using LogQueue = DoubleBufferedQueue<std::string>;
	using LogFilePtr = std::shared_ptr<LogFile>;
	class LogThread
	{
	public:
		LogThread(std::string fileName, const size_t rollSize)
			:spLogFile(std::make_shared<LogFile>(fileName, rollSize))
		{
				isFileExsit_ = spLogFile->create();
		}
		LogThread(FILE* file) :spLogFile(std::make_shared<LogFile>(file))
		{

		}
		void run()
		{
			if (!isFileExsit_)
			{
				return;
			}
			while (true)
			{
				auto spMessage = queue.pop();
				spLogFile->append(spMessage.c_str(), spMessage.size());
			}
		}
	public:
		bool isFileExsit_;
		LogQueue queue;
		LogFilePtr spLogFile;
	};
public:
	AsyncLog();

	static AsyncLog& getInstance();

	~AsyncLog();

	void put(const char* moudle, std::string&& strlog);

	void addLogFile(const std::string &strMoudle, const std::string& strFileName);

	void addLogFile(const std::string &strMoudle,FILE* flie);

	void run();
	 
	void flush();

	void setCancel()
	{
		isCancel_ = true;
	}
	LogLevel getLevel()
	{
		return level_;
	}
private:
	std::unique_ptr<ThreadRAII> spThread_;
	std::map<std::string, std::shared_ptr<LogThread> > logThreads_;
	std::vector<ThreadRAII> threads_;
	bool isCancel_;
	LogLevel level_;
	size_t rollSize_;
};
class LogWrap 
{
private:
	const char * levelToString(LogLevel level)
	{
		switch (level)
		{
		case LogLevel::DEBUG:
			return "[DEBUG]";
		case LogLevel::INFO:
			return "[INFO]";
		case LogLevel::WARNING:
			return "[WARNING]";
		case LogLevel::ERROR:
			return "[ERROR]";
		case LogLevel::FATEL:
			return "[FATEL]";
		default:
			return "[UNKNOWEN]";
		}
	}
public:
	LogWrap(LogLevel level, const char* module):module_(module)
	{
		isWrite_ = level >= AsyncLog::getInstance().getLevel();
		if (isWrite_)
		{
			ss_ = std::unique_ptr<std::stringstream> (new std::stringstream);
			*ss_ << NsTime::GetStrTimeStamp() << ' ' << levelToString(level) << ' ' << std::this_thread::get_id() << ' ';
		}
	}
	template<typename T>
	LogWrap& operator<<(T&& t)
	{
		if (isWrite_ && ss_) {
			*ss_ << std::forward<T>(t);
		}
		return *this;
	}

	~LogWrap()
	{
		if (isWrite_ && ss_)
		{
			*ss_ << '\n';
			AsyncLog::getInstance().put(module_, ss_->str());
		}
	}
private:
	const char * module_;
	bool isWrite_;
	std::unique_ptr<std::stringstream> ss_;
	
};

#define LOG_INFO()  LogWrap(LogLevel::INFO , "default") 
#define LOG_DEBUG()  LogWrap(LogLevel::DEBUG , "default") 
#define LOG_ERROR()  LogWrap(LogLevel::INFO , "default") 