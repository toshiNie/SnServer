#pragma once
#include"stdafx.h"
#include<atomic>
template<typename T>
class DoubleBufferedQueue
{
	void put(T value)
	{

	}
private:
	std::queue<T> queues_[2]; //0 for read  1 for write
	std::atomic_int index_;
};