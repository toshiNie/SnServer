#pragma once
#include"stdafx.h"
#include"Time.h"
#include"noncopyable.h"
#include"FileUtil.h"
class LogFile: public noncopyable
{
	enum {BufferSize= 10 * 1024 * 1024};
public:
	LogFile(const std::string& strBaseName, const size_t rollSize = 1024 * 1024 * 100);

	LogFile(const FILE* file);

	bool create();

	void append(const char* data, size_t size);

	void appendUnlock(const char* data, size_t size);

	void flush();

private:
	bool rollFile();
	
private:
	std::shared_ptr<WritableFile>  spWriteableFile_;
	std::string strBaseName_;
	std::vector<std::string> fileNames_;
	size_t size_;
	const size_t rollSize_;
	size_t flushSize_;
	bool needCreate_;
};
typedef std::shared_ptr<LogFile> LogFilePtr;