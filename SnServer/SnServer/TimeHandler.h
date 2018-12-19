#pragma once
#include "stdafx.h"
#include <sys/timerfd.h>
#include "EventHandle.h"
#include "Reactor.h"


class TimeEvent
{
public:
	TimeEvent(const std::function<void()>&& func)
		:timefd_(timerfd_create(CLOCK_MONOTONIC, TFD_CLOEXEC | TFD_NONBLOCK)),
		func_(func)
	{

	}
	bool setTime(int milliseconds)
	{
		struct itimerspec its;
		its.it_value.tv_sec = milliseconds / 1000;
		its.it_value.tv_nsec = (milliseconds % 1000) * 1000000;
		its.it_interval.tv_sec = milliseconds / 1000;
		its.it_interval.tv_nsec = (milliseconds % 1000) * 1000000;

		if (timerfd_settime(timefd_, 0, &its, NULL) < 0)
		{
			return false;
		}
		return true;
	}
	int getFd() { return timefd_; }

	void operator()()
	{
		func_();
	}
private:
	int timefd_;
	std::function<void()> func_;
};

typedef std::shared_ptr<TimeEvent> TimeEventPtr;

class TimeHandler :public EventHandler
{
public:
	TimeHandler(TimeEventPtr spTimeEvent ,ReactorPtr spReactor) :
		spReactor_(spReactor),
		spTimeEvent_(spTimeEvent)
	{
		SetHandlerType(ReadEvent);
	}

	void ReadHandle() override
	{
		(*spTimeEvent_)();
		spTimeEvent_->setTime(2000);
		spReactor_->AddHandler(shared_from_this());
	}
	int GetFd() override
	{
		return spTimeEvent_->getFd();
	}
private:
	ReactorPtr spReactor_;
	TimeEventPtr spTimeEvent_;
};