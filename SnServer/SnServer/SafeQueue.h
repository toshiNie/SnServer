#pragma once
#include"stdafx.h"
template<typename T>
class SafeQueue
{
public:
	SafeQueue(size_t maxsize = -1) :
		maxSize_(maxsize)
	{

	}
	//void push(T& x)
	//{
	//	std::lock_guard<std::mutex> guard(mutex_);
	//	while (isFullWithoutLock())
	//	{
	//		condNotFull_.wait();
	//	}
	//	queue_.push(x);
	//	condNotEmpty_.notify_one();
	//}

	void push(T&& x)
	{
		std::unique_lock<std::mutex> lck(mutex_);
		while (isFullWithoutLock())
		{
			condNotFull_.wait(lck);
		}
		queue_.push(std::move(x));
		condNotEmpty_.notify_one();
	}

	T pop()
	{
		std::unique_lock<std::mutex> lck(mutex_);
		while (isEmptyWithoutLock())
		{
			condNotEmpty_.wait(lck);
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
		if (maxSize_ == 0)
		{
			return false;
		}
		return queue_.size() >= maxSize_;
	}
	bool isEmptyWithoutLock()
	{
		return queue_.size() == 0;
	}
};