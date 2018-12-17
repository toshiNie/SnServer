#pragma once
#include"stdafx.h"
#include"Time.h"
#include"noncopyable.h"
#include"FileUtil.h"
class LogFile: public noncopyable
{
	enum {BufferSize=1024};
public:
	LogFile(const std::string& strBaseName, const size_t rollSize = 1024 * 100) :strBaseName_(strBaseName), size_(0), rollSize_(rollSize)
	{

	}
	bool Create()
	{
		return RollFile();
	}
	void Append(const char* data, size_t size)
	{
		std::lock_guard<std::mutex> lock(mutex_);
		AppendUnlock(data, size);
	}
	void AppendUnlock(const char* data, size_t size)
	{
		std::string strTimeTemp = NsTime::GetStrTimeStamp();
		spWriteableFile_->Append(strTimeTemp.c_str(),strTimeTemp.size());
		spWriteableFile_->Append(data, size);
		size_ += size;
		if (size_ >= rollSize_)
		{
			RollFile();
		}
		//todo 

	}
private:
	bool RollFile()
	{
		std::string strFileName = strBaseName_ + NsTime::GetStrTimeStamp(".%Y-%m-%d.")+std::to_string(fileNames_.size());
		spWriteableFile_.reset(new PosixWritableFile(strFileName.c_str()));
		if (!spWriteableFile_->Open())
		{
			std::cout << "open log file error" << std::endl;
			return false;
		}
		fileNames_.push_back(std::move(strFileName));
		return true;
	}
	

private:
	std::shared_ptr<WritableFile>  spWriteableFile_;
	std::mutex mutex_;
	std::string strBaseName_;
	std::vector<std::string> fileNames_;
	size_t size_;
	const size_t rollSize_;

private:
};
typedef std::shared_ptr<LogFile> LogFilePtr;