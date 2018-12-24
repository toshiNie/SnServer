#include "stdafx.h"
#include "LogFile.h"


LogFile::LogFile(const std::string& strBaseName, const size_t rollSize)
	:strBaseName_(strBaseName),
	size_(0),
	rollSize_(rollSize),
	needCreate_(true),
	flushSize_(0)
{

}
LogFile::LogFile(const FILE* file)
	:spWriteableFile_(new PosixWritableFile(file)),
	rollSize_(-1), needCreate_(false),
	size_(0),
	flushSize_(0)
{

}
bool LogFile::Create()
{
	if (needCreate_)
	{
		return RollFile();
	}
	return true;
}
void LogFile::Append(const char* data, size_t size)
{
	//std::lock_guard<std::mutex> lock(mutex_);
	AppendUnlock(data, size);
}
void LogFile::AppendUnlock(const char* data, size_t size)
{
	spWriteableFile_->append(data, size);
	size_ += size;
	flushSize_ += size;

	if (flushSize_ > BufferSize)
	{
		//std::cout << "flush" << std::endl;
		spWriteableFile_->flush();
		flushSize_ = 0;
	}
	if (size_ >= rollSize_)
	{
		RollFile();
	}
	//todo 
}
void LogFile::Flush()
{
	spWriteableFile_->flush();
}


bool LogFile::RollFile()
{
	std::string strFileName;
	while (true)
	{
		strFileName = strBaseName_ + NsTime::GetStrTimeStamp(".%Y-%m-%d.") + std::to_string(fileNames_.size());
		spWriteableFile_.reset(new PosixWritableFile(strFileName.c_str()));
		if (spWriteableFile_->isExist())
		{
			if (spWriteableFile_->size() < rollSize_)
			{
				break;
			}
			else
			{
				fileNames_.push_back(std::move(strFileName));
			}
		}
		else
		{
			break;
		}
	}
	if (!spWriteableFile_->open())
	{
		std::cout << "open log file error" << std::endl;
		return false;
	}
	fileNames_.push_back(std::move(strFileName));
	size_ = spWriteableFile_->size();
	return true;
}