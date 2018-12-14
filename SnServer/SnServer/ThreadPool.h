#pragma once
#include"stdafx.h"



class CThreadPool
{

	typedef std::shared_ptr<std::thread> ThreadPtr;
public:
	typedef std::function<void()> Task;
	CThreadPool()
		:isRunning_(false)
	{

	}
	~CThreadPool()
	{
		for (auto&sp : threads_)
		{
			sp->join();
		}
		isRunning_ = false;
	}
	void Start()
	{
		isRunning_ = true;
		while (!waitTasks_.empty())
		{
			threads_.emplace_back(ThreadPtr(new std::thread(std::bind(&CThreadPool::RunInThread, this))));
		}
	}
	void PushPack(Task &&task)
	{
		std::lock_guard<std::mutex> lock(mutex_);
		waitTasks_.push(task);
		cvTask_.notify_one();
	}
	void stop()
	{
		for (auto& thread : threads_)
		{
			thread->join();
		}
		isRunning_ = false;
	}
	bool isFull()
	{
		return waitTasks_.size() >= threadNumber_;
	}
	bool isRunning()
	{
		return isRunning_;
	}
	Task Take()
	{
		std::unique_lock<std::mutex> lock(mutex_);
		while (waitTasks_.empty())
		{
			cvTask_.wait(lock);
		}
		if (!waitTasks_.empty())
		{
			auto spTask = waitTasks_.front();
			waitTasks_.pop();
			return spTask;
		}
		return nullptr;
	}

private:
	void RunInThread()
	{
		while (isRunning_)
		{
			auto task = Take();
			if(task)
				(task)();
		}
	}
private:
	std::vector<ThreadPtr> threads_;
	size_t threadNumber_;
	std::mutex mutex_;
	std::condition_variable  cvTask_;
	std::queue<Task> waitTasks_;
	bool isRunning_;
};