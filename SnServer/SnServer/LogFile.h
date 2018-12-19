#pragma once
#include"stdafx.h"
#include"Time.h"
#include"noncopyable.h"
#include"FileUtil.h"
class LogFile: public noncopyable
{
	enum {BufferSize= 64 * 1024};
public:
	LogFile(const std::string& strBaseName, const size_t rollSize = 1024 * 1024 * 100);

	LogFile(const FILE* file);

	bool Create();

	void Append(const char* data, size_t size);

	void AppendUnlock(const char* data, size_t size);

	void Flush();

private:
	bool RollFile();
	
private:
	std::shared_ptr<WritableFile>  spWriteableFile_;
	std::mutex mutex_;
	std::string strBaseName_;
	std::vector<std::string> fileNames_;
	size_t size_;
	const size_t rollSize_;
	size_t flushSize_;
	bool needCreate_;
};
typedef std::shared_ptr<LogFile> LogFilePtr;