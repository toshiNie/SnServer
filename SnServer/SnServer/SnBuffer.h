#pragma once
#include"stdafx.h"
class SnBuffer
{
	enum {HeadSize = 0};
public:
	SnBuffer(size_t size = 1024) :buffer_(size), index_(HeadSize), headSize_(HeadSize){}
	void append(const char* data, size_t size)
	{
		if (buffer_.size() < index_ + size)
		{
			buffer_.resize(index_ + size);
		}
		memcpy(buffer_.data() + index_, data, size);
		index_ += size;
	}

	SnBuffer(SnBuffer && rvl)
	{
		buffer_.swap(rvl.buffer_);
		index_ = rvl.index_;
		headSize_ = rvl.headSize_;
	}
	void consumHead(size_t size)
	{
		headSize_ += size;
	}
	size_t size()
	{
		return index_ - headSize_;
	}
	char * getReadbuffer()
	{
		return buffer_.data() + headSize_;
	}
	
	int read(std::vector<char> &buffer, int len)
	{
		if (len > size())
		{
			buffer.swap(buffer_);
			index_ = 0;
			return buffer.size();
		}
		memcpy(buffer.data(), buffer_.data(), len);
		size_t remainSize = size() - len;
		std::vector<char> temp(buffer_.size());
		memcpy(temp.data(), buffer_.data() + len, remainSize);
		buffer_.swap(temp);
		index_ = remainSize;
		return len;
	}

	void reset()
	{
		std::vector<char> vec(buffer_.size());
		buffer_.swap(vec);
		index_ = HeadSize;
	}
private:
	std::vector<char> buffer_;
	size_t index_;
	size_t headSize_;
};



class Buffer
{
public:
	Buffer() {}

	void peek(int size)
	{
	}
private:
	int readIndex_;
	int writeIndex_;
};