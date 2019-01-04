#pragma once
#include"stdafx.h"
class SnBufferOld
{
	enum {HeadSize = 0};
public:
	SnBufferOld(size_t size = 1024) :index_(HeadSize), headSize_(HeadSize),remainSize_(size), bufferSize_(size)
	{
		buffer_.reserve(size);
	}

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

	SnBufferOld(SnBufferOld && rvl)
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
		std::vector<char> temp;
		temp.reserve(remainSize > bufferSize_ ? remainSize:bufferSize_);
		memcpy(temp.data(), buffer_.data() + len, remainSize);
		buffer_.swap(temp);
		index_ = remainSize;
		remainSize_ = buffer_.capacity() - index_;
		return len;
		//std::cout << "3." << index_ << ":" << remainSize_ << ":" << buffer_.size() << ":" << buffer_.capacity() << ":" << len << std::endl;;
	}

	size_t expand()
	{
		buffer_.reserve(index_ * 2);
		remainSize_ = buffer_.capacity() - index_;
		return remainSize_;
	}

	void reset()
	{
		buffer_.clear();
		index_ = 0;
		remainSize_ = buffer_.capacity();
	}
private:
	std::vector<char> buffer_;
	size_t index_;
	size_t headSize_;
	size_t remainSize_;
	size_t bufferSize_;
};



class SnBuffer
{
public:
	SnBuffer(size_t size = 1024) : readIndex_(0),writeIndex_(0),buffer_(size)
	{

	}

	void append(const char* data, size_t size)
	{
		if (buffer_.size() - writeIndex_ < size)
		{
			buffer_.resize(buffer_.size() + size);
		}
		memcpy(buffer_.data() + writeIndex_, data, size);
		writeIndex_ += size;
	}
	size_t size()
	{
		return writeIndex_ - readIndex_;
	}
	size_t getRemainSize()
	{
		return buffer_.size() - writeIndex_;
	}
	
	char * getReadbuffer()
	{
		return buffer_.data() + readIndex_;
	}

	char * getRemainbuffer()
	{
		return buffer_.data() + writeIndex_;
	}

	void peek(size_t size)
	{
		writeIndex_ += size;
	}

	void consumHead(size_t size)
	{
		readIndex_ += size;
	}

	int read(std::vector<char> &buffer, int len)
	{
		memcpy(buffer.data(), buffer_.data() + readIndex_, len);
		readIndex_ += len;
		if (readIndex_ == writeIndex_)
		{
			readIndex_ = writeIndex_ = 0;
		}
		return len;
	}
	void reset()
	{
		buffer_.clear();
		readIndex_ = writeIndex_ = 0;
	}
	size_t expand()
	{
		buffer_.resize(buffer_.size() * 2);
		return buffer_.size() - writeIndex_;
	}
private:
	std::vector<char> buffer_;
	size_t readIndex_;
	size_t writeIndex_;

};