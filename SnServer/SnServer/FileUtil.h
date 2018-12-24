#pragma once
#include"stdafx.h"
#pragma warning(disable:4996)
class WritableFile {
public:
	WritableFile() {}
	virtual ~WritableFile() {};
	virtual bool open() = 0;
	virtual bool append(const char* data,size_t size) = 0;
	virtual bool flush() = 0;
	virtual bool isExist() = 0;
	virtual int size() = 0;

private:
	WritableFile(const WritableFile&);
	void operator=(const WritableFile&);
};



class PosixWritableFile :public WritableFile
{
public:
	PosixWritableFile(const char* fileName) :strFileName_(fileName), file_(NULL)
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
	bool open()
	{
		file_ = ::fopen(strFileName_.c_str(), "ae");

		if (file_ != NULL)
		{
			::setbuffer(file_, buffer_, sizeof(buffer_));
		}

		return file_ != NULL;
	}
	bool append(const char* data, size_t size)
	{
		//size_t s = fwrite(data, 1, size, file_);
		size_t s = fwrite_unlocked(data, 1, size, file_);
		if (size != s)
			return false;
		return true;
	}
	bool flush()
	{
		if (::fflush(file_) != 0)
			return false;
		return true;
	}
	bool isExist()
	{
		if (access(strFileName_.c_str(), F_OK) ==  -1)
		{
			return false;
		}
		return true;
	}
	int size()
	{
		struct stat statbuf;
		bzero(&statbuf, sizeof(statbuf));
		stat(strFileName_.c_str(), &statbuf);
		return statbuf.st_size;
	}
private:
	FILE* file_;
	char buffer_[64 * 1024];
	std::string strFileName_;
};
