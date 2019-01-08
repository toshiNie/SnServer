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
	:spWriteableFile_(std::make_shared<PosixWritableFile>(file)),
	rollSize_(-1), needCreate_(false),
	size_(0),
	flushSize_(0)
{

}
bool LogFile::create()
{
	if (needCreate_)
	{
		return rollFile();
	}
	return true;
}
void LogFile::append(const char* data, size_t size)
{
	//std::lock_guard<std::mutex> lock(mutex_);
	appendUnlock(data, size);
}
void LogFile::appendUnlock(const char* data, size_t size)
{
	spWriteableFile_->append(data, size);
	size_ += size;
	flushSize_ += size;

	if (flushSize_ > BufferSize)
	{
		std::cout << "flush:" <<flushSize_ <<std::endl;
		spWriteableFile_->flush();
		flushSize_ = 0;
	}
	if (size_ >= rollSize_)
	{
		rollFile();
	}
	//todo 
}
void LogFile::flush()
{
	spWriteableFile_->flush();
}


bool LogFile::rollFile()
{
	std::string strFileName;
	while (true)
	{
		strFileName = strBaseName_ + NsTime::GetStrTimeStamp(".%Y-%m-%d.") + std::to_string(fileNames_.size());
		spWriteableFile_ = std::make_shared<PosixWritableFile>(strFileName.c_str());
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
	fileNames_.emplace_back(std::move(strFileName));
	size_ = spWriteableFile_->size();
	return true;
}