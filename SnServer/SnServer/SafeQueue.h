#pragma once
#include"stdafx.h"
template<typename T>
class SafeQueue
{
	SafeQueue(size_t maxsize) :
		maxSize_(maxsize),
		mutex_(),
		condNotEmpty_(mutex_),
		condNotFull_(mutex_),
		queue_(maxSize_)
	{

	}
	void push(T& x)
	{
		std::lock_guard<std::mutex> guard(mutex_);
		while (isFullWithoutLock())
		{
			condNotFull_.wait();
		}
		queue_.push(x);
		condNotEmpty_.notify_one();

	}
	T pop()
	{
		std::lock_guard<std::mutex> guard(mutex_);
		while (isEmptyWithoutLock())
		{
			condNotEmpty_.wait();
		}
		T temp(queue_.front());
		queue_.pop();
		condNotFull_.notify_one();
		return temp;
	}
	bool isFull()
	{
		std::lock_guard<std::mutex> guard(mutex_);
		return isFullWithoutLock();
	}
	bool isEmpty()
	{
		std::lock_guard<std::mutex> guard(mutex_);
		return isEmptyWithoutLock();
	}
	size_t size()
	{
		std::lock_guard<std::mutex> guard(mutex_);
		return queue_.size();
	}

private:
	std::queue<T> queue_;
	std::mutex mutex_;
	std::condition_variable condNotEmpty_;
	std::condition_variable condNotFull_;
	size_t maxSize_;
private:
	bool isFullWithoutLock()
	{
		return queue_.size() >= maxSize_;
	}
	bool isEmptyWithoutLock()
	{
		return queue_.size() == 0;
	}
};