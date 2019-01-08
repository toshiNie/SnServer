#pragma once
#include"stdafx.h"
#pragma warning(disable:4996)
#include"noncopyable.h"
class WritableFile : public noncopyable{
public:
	WritableFile() {}
	virtual ~WritableFile() {};
	virtual bool open() = 0;
	virtual bool append(const char* data,size_t size) = 0;
	virtual bool flush() = 0;
	virtual bool isExist() = 0;
	virtual int size() = 0;
};



class PosixWritableFile :public WritableFile
{
public:
	PosixWritableFile(const char* fileName) :strFileName_(fileName), file_(nullptr)
	{
		
	}
	PosixWritableFile(const FILE* file) :strFileName_(), file_(const_cast<FILE*>(file))
	{
		//::setbuffer(file_, buffer_, sizeof(buffer_));
	}
	~PosixWritableFile()
	{
		if (file_)
		{
			::fclose(file_);
		}
	}
	bool open() override
	{
		file_ = ::fopen(strFileName_.c_str(), "ae");

		if (file_)
		{
			::setbuffer(file_, buffer_, sizeof(buffer_));
		}
		return file_ != nullptr;
	}
	bool append(const char* data, size_t size) override
	{
		//size_t s = fwrite(data, 1, size, file_);
		size_t s = fwrite_unlocked(data, 1, size, file_);
		if (size != s)
			return false;
		return true;
	}
	bool flush() override
	{
		if (::fflush(file_) != 0)
			return false;
		printf("flush\n");
		return true;
	}
	bool isExist() override
	{
		if (access(strFileName_.c_str(), F_OK) ==  -1)
		{
			return false;
		}
		return true;
	}
	int size() override
	{
		struct stat statbuf;
		bzero(&statbuf, sizeof(statbuf));
		stat(strFileName_.c_str(), &statbuf);
		return statbuf.st_size;
	}
private:
	FILE* file_;
	char buffer_[1024 * 1024];
	std::string strFileName_;
};
