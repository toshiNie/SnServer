#pragma once
#include"stdafx.h"
class SnBuffer
{
public:
	SnBuffer(size_t size) :buffer_(size) {}
	void Append(const char* data, size_t size)
	{
		
	}
private:
	std::vector<char> buffer_;
	std::vector<char>::iterator iter_;
	size_t index_;
};