#include"stdafx.h"
#include "Reactor.h"
#include"TimeWheel.h"

TimeWheel::TimeWheel(int size) :bucket_(size), index_(0)
{

}

void TimeWheel::setFunc(std::function<void(int)> && func)
{
	func_ = func;
}

void TimeWheel::onTimer()
{
	for (auto &sock : bucket_[index_])
	{
		func_(sock);
	}
	bucket_[index_].clear();
	index_ = ++index_ % bucket_.size();
}

TimeEventPtr TimeWheel::getEvent(int interval)
{
	auto spTimerEvent = std::make_shared<TimeEvent>(std::bind(&TimeWheel::onTimer, this));
	spTimerEvent->setTime(interval);
	return spTimerEvent;
}

int TimeWheel::addSock(int sock, int& refIndex)
{
	int index = index_ - 1;
	if (index < 0)
	{
		index = bucket_.size() - 1;
	}
	bucket_[index].insert(sock);
	refIndex = index;
	return index;
}
int TimeWheel::resetSock(int sock, int& refIndex)
{
	if (refIndex == -1)
	{
		return addSock(sock, refIndex);
	}
	if (refIndex == index_)
	{
		return -1;
	}
	bucket_[refIndex].erase(sock);
	return addSock(sock, refIndex);
}

void TimeWheel::remove(int sock, int& refIndex)
{
	if (refIndex == index_)
	{
		return;
	}
	bucket_[refIndex].erase(sock);
}