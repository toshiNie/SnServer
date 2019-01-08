#include"stdafx.h"
#include"TimeEvent.h"

TimeEvent::TimeEvent(std::function<void()>&& func)
	:timefd_(timerfd_create(CLOCK_MONOTONIC, TFD_CLOEXEC | TFD_NONBLOCK)),
	func_(func),
	cancleFlag_(false)
{

}
bool TimeEvent::setTime(int milliseconds)
{
	interval_ = milliseconds;
	struct itimerspec its;
	its.it_value.tv_sec = milliseconds / 1000;
	its.it_value.tv_nsec = (milliseconds % 1000) * 1000000;
	its.it_interval.tv_sec = milliseconds / 1000;
	its.it_interval.tv_nsec = (milliseconds % 1000) * 1000000;

	if (timerfd_settime(timefd_, 0, &its, nullptr) < 0)
	{
		return false;
	}
	return true;
}
void TimeEvent::cancle()
{
	cancleFlag_ = true;
}
bool TimeEvent::getCancleFlag()
{
	return cancleFlag_;
}
int TimeEvent::getFd() { return timefd_; }
int TimeEvent::getInterval()
{
	return interval_;
}
void TimeEvent::operator()()
{
	func_();
}