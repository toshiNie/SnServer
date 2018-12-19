#pragma once
#include"stdafx.h"
#pragma warning(disable:4996)
class WritableFile {
public:
	WritableFile() { }
	virtual ~WritableFile() {};
	virtual bool Open() = 0;
	virtual bool Append(const char* data,size_t size) = 0;
	virtual bool Flush() = 0;
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
	bool Open()
	{
		file_ = ::fopen(strFileName_.c_str(), "ae");
		if (file_ != NULL)
		{
			::setbuffer(file_, buffer_, sizeof(buffer_));
		}
		return file_ != NULL;
	}
	bool Append(const char* data, size_t size)
	{
		//size_t s = fwrite(data, 1, size, file_);
		size_t s = fwrite_unlocked(data, 1, size, file_);
		if (size != s)
			return false;
		return true;
	}
	bool Flush()
	{
		if (::fflush(file_) != 0)
			return false;
		return true;
	}
private:
	FILE* file_;
	char buffer_[64 * 1024];
	std::string strFileName_;
};
