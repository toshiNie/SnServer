#include"stdafx.h"
#include "Reactor.h"
#include"TimeHandler.h"


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

	if (timerfd_settime(timefd_, 0, &its, NULL) < 0)
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



TimeHandler::TimeHandler(TimeEventPtr spTimeEvent, ReactorPtr spReactor) :
	spReactor_(spReactor),
	spTimeEvent_(spTimeEvent)
{
	SetHandlerType(ReadEvent);
}

void TimeHandler::ReadHandle() 
{
	uint64_t time;
	int r = read(spTimeEvent_->getFd(), (void*)&time, sizeof(time));
	if (r > 0)
	{
		//std::cout << time << std::endl;
	}
	(*spTimeEvent_)();
	if (spTimeEvent_->getCancleFlag())
	{
		spReactor_->Remove(spTimeEvent_->getFd());
		::close(spTimeEvent_->getFd());
	}
	else
	{
		spTimeEvent_->setTime(spTimeEvent_->getInterval());
		spReactor_->AddHandler(shared_from_this());
	}
}
int TimeHandler::GetFd() 
{
	return spTimeEvent_->getFd();
}