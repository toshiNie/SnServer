#pragma once
#include"stdafx.h"
class SnBuffer
{
	enum {HeadSize = 0};
public:
	SnBuffer(size_t size = 1024) :buffer_(size), index_(HeadSize){}
	void append(const char* data, size_t size)
	{
		if (buffer_.size() < index_ + size)
		{
			buffer_.resize(buffer_.size() * 2);
		}
		std:memmove(buffer_.data() + index_, data, size);
	}
	size_t size()
	{
		return index_;
	}
	const char * getbuffer()
	{
		return buffer_.data() + index_;
	}
	void reset()
	{
		buffer_.clear();
		index_ = HeadSize;
	}
private:
	std::vector<char> buffer_;
	std::vector<char>::iterator iter_;	
	size_t index_;
};