#pragma once
#include"stdafx.h"



class CThreadPool
{

	typedef std::shared_ptr<std::thread> ThreadPtr;
public:
	typedef std::function<void()> Task;
	typedef std::shared_ptr<Task> TaskPtr;
	CThreadPool()
		:isRunning_(false)
	{

	}
	void Start(int num)
	{
		threadNumber_ = num;
		isRunning_ = true;
		for (int i = 0; i < num; i++)
		{
			threads_.emplace_back(ThreadPtr(new std::thread(std::bind(&CThreadPool::RunInThread,this))));
		}
	}
	void PushPack(TaskPtr &&task)
	{
		if (threadNumber_ == 0)
		{
			if(task)
				(*task)();
		}
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
	TaskPtr Take()
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
				(*task)();
		}
	}
private:
	std::vector<ThreadPtr> threads_;
	size_t threadNumber_;
	std::mutex mutex_;
	std::condition_variable  cvTask_;
	std::queue<TaskPtr> waitTasks_;
	bool isRunning_;
};