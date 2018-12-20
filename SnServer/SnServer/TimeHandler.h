#pragma once
#include <stdint.h>
#include <sys/timerfd.h>
#include "EventHandle.h"



class TimeEvent
{
public:
	TimeEvent(std::function<void()>&& func);
	bool setTime(int milliseconds);
	void cancle();
	bool getCancleFlag();
	int getFd();
	int getInterval();
	void operator()();

private:
	int timefd_;
	bool cancleFlag_;
	std::function<void()> func_;
	int interval_;
};

typedef std::shared_ptr<TimeEvent> TimeEventPtr;

class TimeHandler
	:public EventHandler
{
public:
	TimeHandler(TimeEventPtr spTimeEvent, ReactorPtr spReactor);
	void ReadHandle() override;
	int GetFd() override;
private:
	ReactorPtr spReactor_;
	TimeEventPtr spTimeEvent_;
};