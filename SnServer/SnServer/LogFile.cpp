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
	std::string strTimeTemp = NsTime::GetStrTimeStamp();
	spWriteableFile_->Append(strTimeTemp.c_str(), strTimeTemp.size());
	spWriteableFile_->Append(data, size);
	size_ += (size + strTimeTemp.size());
	flushSize_ += (size + strTimeTemp.size());

	if (flushSize_ > BufferSize)
	{
		//std::cout << "flush" << std::endl;
		spWriteableFile_->Flush();
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
	spWriteableFile_->Flush();
}


bool LogFile::RollFile()
{
	std::string strFileName = strBaseName_ + NsTime::GetStrTimeStamp(".%Y-%m-%d.") + std::to_string(fileNames_.size());
	spWriteableFile_.reset(new PosixWritableFile(strFileName.c_str()));
	if (!spWriteableFile_->Open())
	{
		std::cout << "open log file error" << std::endl;
		return false;
	}
	fileNames_.push_back(std::move(strFileName));
	size_ = 0;
	return true;
}