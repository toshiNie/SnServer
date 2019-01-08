#include "stdafx.h"
#include "AsyncLog.h"
#include"ReadThread.h"
#include "Reactor.h"

Reactor::Reactor() :upNsEpoll_(new NsEpoll)
{
}
void Reactor::addHandler(EventHandlerPtr spEventHandler)
{
	LOG_DEBUG("add sock:" + std::to_string(spEventHandler->getFd()));
	auto iter = mapHandler_.find(spEventHandler->getFd());
	if (iter == mapHandler_.end())
	{
		mapHandler_[spEventHandler->getFd()] = spEventHandler;
		upNsEpoll_->addEvent(spEventHandler->getFd(), spEventHandler->getHandlerType());
	}
	else
	{
		mapHandler_[spEventHandler->getFd()] = spEventHandler;
		upNsEpoll_->mod(spEventHandler->getFd(), spEventHandler->getHandlerType());
	}
}
void Reactor::remove(int fd)
{
	auto iter = mapHandler_.find(fd);
	if (iter != mapHandler_.end())
	{
		mapHandler_.erase(iter);
		upNsEpoll_->remove(fd);
	}
}

void Reactor::mod(int fd, Event event)
{
	auto iter = mapHandler_.find(fd);
	if (iter != mapHandler_.end())
	{
		upNsEpoll_->mod(fd, event);
	}
}
void Reactor::loop(int timeout)
{
	upNsEpoll_->waitEvent(mapHandler_, timeout);
}