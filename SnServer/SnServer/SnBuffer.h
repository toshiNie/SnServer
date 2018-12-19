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
			buffer_.resize(buffer_.size() * 2 + size);
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
	const char * getbuffer()
	{
		return buffer_.data() + headSize_;
	}
	void reset()
	{
		std::vector<char> vec(1024);
		buffer_.swap(vec);
		index_ = HeadSize;
	}
private:
	std::vector<char> buffer_;
	size_t index_;
	size_t headSize_;
};