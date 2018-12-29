#pragma once
#include <stdint.h>
#include <sys/timerfd.h>

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
using TimeEventPtr = std::shared_ptr<TimeEvent>;