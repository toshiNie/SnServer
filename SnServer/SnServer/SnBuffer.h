#pragma once
#include"stdafx.h"
class SnBuffer
{
	enum {HeadSize = 0};
public:
	SnBuffer(size_t size = 1024) :buffer_(size), index_(HeadSize), headSize_(HeadSize){}
	void append(const char* data, size_t size)
	{
		std::cout << buffer_.size()<<std::endl;
		if (buffer_.size() < index_ + size)
		{
			buffer_.resize(buffer_.size() * 2);
		}
		memcpy(buffer_.data() + index_, data, size);
		index_ += size;
	}

	void consumHead(size_t size)
	{
		headSize_ += size;
	}
	size_t size()
	{
		return index_;
	}
	const char * getbuffer(size_t headSize = 0)
	{
		return buffer_.data() + headSize;
	}
	void reset()
	{
		std::vector<char> vec(1024);
		buffer_.swap(vec);
		index_ = HeadSize;
	}
private:
	std::vector<char> buffer_;
	std::vector<char>::iterator iter_;	
	size_t index_;
	size_t headSize_;
};