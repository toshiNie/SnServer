#pragma once
#include"stdafx.h"
#define STATIC_CHECK(expr) {char unnamed[expr? 1 : 0];}
template<typename T>
class SafeQueue
{
public:
	SafeQueue(size_t maxsize = -1) :
		maxSize_(maxsize)
	{

	}

	template<typename U>
	void push(U&& x)
	{
		STATIC_CHECK(sizeof(U) <= sizeof(T));
		std::unique_lock<std::mutex> lck(mutex_);
		/*	while (isFullWithoutLock())
		{
		condNotFull_.wait(lck);
		}*/
		queue_.push(std::forward<T>(x));
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