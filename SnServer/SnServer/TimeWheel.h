#pragma once
#include"stdafx.h"
#include <atomic>
#include"TimeHandler.h"

class TimeWheel
{
public:
	TimeWheel(int size = 8):bucket_(size), index_(0)
	{

	}

	void setFunc(std::function<void(ConnectSessionPtr)> && func)
	{
		func_ = func;
	}

	void onTimer()
	{
		for (auto &spConnect : bucket_[index_])
		{
			func_(spConnect);
		}
		bucket_[index_].clear();
		index_ = ++index_ % bucket_.size();
	}

	TimeEventPtr getEvent(int interval)
	{
		TimeEventPtr spTimerEvent(new TimeEvent(std::bind(&TimeWheel::onTimer, this)));
		spTimerEvent->setTime(interval);
		return spTimerEvent;
	}

	int addSock(ConnectSessionPtr spConnect)
	{
		int index = index_ - 1;
		if (index < 0)
		{
			index = bucket_.size() - 1;
		}
		bucket_[index].insert(spConnect);
		spConnect->setIndex(index);
		return index;
	}
	int resetSock(ConnectSessionPtr spConnect)
	{
		if (spConnect->getIndex() == -1)
		{
			return addSock(spConnect);
		}
		if (spConnect->getIndex() == index_)
		{
			return -1;
		}
		bucket_[spConnect->getIndex()].erase(spConnect);
		return addSock(spConnect);
	}
	
	void remove(ConnectSessionPtr spConnect)
	{
		if (spConnect->getIndex() == index_)
		{
			return ;
		}
		bucket_[spConnect->getIndex()].erase(spConnect);
	}
private:
	std::vector<std::set<ConnectSessionPtr>> bucket_;
	std::atomic_int index_;
	std::function<void(ConnectSessionPtr)> func_;
	TimeEventPtr spEvent_;
};