#pragma once
#include"stdafx.h"
class SnBuffer
{
	enum {HeadSize = 0};
public:
	SnBuffer(size_t size = 1024) :buffer_(size), index_(HeadSize), headSize_(HeadSize),remainSize_(size){}

	void append(const char* data, size_t size)
	{
		//std::cout << "1.s" << index_ << ":" << remainSize_ << ":" << buffer_.size() << ":" << buffer_.capacity() << ":" << size << std::endl;
		if (buffer_.size() < index_ + size)
		{
			buffer_.resize(index_ + size);
			remainSize_ = buffer_.capacity() - index_;
		}
		memcpy(buffer_.data() + index_, data, size);
		index_ += size;
		remainSize_ -= size;
		//std::cout << "1." << index_ << ":" << remainSize_ << ":" << buffer_.size() << ":" << buffer_.capacity() << ":" << size << std::endl;
	}
	void peek(size_t size)
	{
		//std::cout << "2.s" << index_ << ":" << remainSize_ << ":" << buffer_.size() << ":" << buffer_.capacity() << ":" << size << std::endl;
		index_ += size;
		remainSize_ -= size;
		//std::cout << "2." << index_ << ":" << remainSize_ << ":" << buffer_.size() << ":" << buffer_.capacity() << ":" << size << std::endl;
	}

	SnBuffer(SnBuffer && rvl)
	{
		buffer_.swap(rvl.buffer_);
		index_ = rvl.index_;
		headSize_ = rvl.headSize_;
		remainSize_ = rvl.remainSize_;
	}
	void consumHead(size_t size)
	{
		headSize_ += size;
	}
	size_t size()
	{
		return index_ - headSize_;
	}
	size_t getRemainSize()
	{
		return remainSize_;
	}
	char * getReadbuffer()
	{
		return buffer_.data() + headSize_;
	}
	char * getRemainbuffer()
	{
		return buffer_.data() + index_;
	}
	
	int read(std::vector<char> &buffer, int len)
	{
		//std::cout << "3.s" << index_ << ":" << remainSize_ << ":" << buffer_.size() << ":" << buffer_.capacity() << ":" << len << std::endl;;
		if (len > size())
		{
			buffer.swap(buffer_);
			index_ = 0;
			return buffer.size();
		}
		memcpy(buffer.data(), buffer_.data(), len);
		size_t remainSize = size() - len;
		std::vector<char> temp(remainSize);
		memcpy(temp.data(), buffer_.data() + len, remainSize);
		buffer_.swap(temp);
		index_ = remainSize;
		remainSize_ = buffer_.capacity() - index_;
		return len;
		//std::cout << "3." << index_ << ":" << remainSize_ << ":" << buffer_.size() << ":" << buffer_.capacity() << ":" << len << std::endl;;
	}

	size_t expand()
	{
		buffer_.resize(index_ * 2);
		remainSize_ = buffer_.capacity() - index_;
		return remainSize_;
	}

	void reset()
	{
		std::vector<char> vec(buffer_.size());
		buffer_.swap(vec);
		index_ = HeadSize;
		remainSize_ = buffer_.capacity();
	}
private:
	std::vector<char> buffer_;
	size_t index_;
	size_t headSize_;
	size_t remainSize_;
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