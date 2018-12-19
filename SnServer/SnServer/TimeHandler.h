#pragma once
#include "stdafx.h"
#include <sys/timerfd.h>
#include "EventHandle.h"
#include "Reactor.h"


class Timer
{
public:

};


class TimeHandler :public EventHandler
{
public:
	TimeHandler(std::function<void()>&& func,ReactorPtr spReactor) :
		timefd_(timerfd_create(CLOCK_MONOTONIC, TFD_CLOEXEC| TFD_NONBLOCK)),
		func_(func),
		spReactor_(spReactor)
	{

	}
	void ReadHandler() 
	{
		func_();
	}

private:
	int timefd_;
	std::function<void()> func_;
	ReactorPtr spReactor_;
};