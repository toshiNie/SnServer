#include "stdafx.h"
#include "AsyncLog.h"
#include"ReadThread.h"
#include "Reactor.h"

Reactor::Reactor() :upEpoll_(new Epoll)
{
}
void Reactor::addHandler(EventHandlerPtr spEventHandler)
{
	LOG_DEBUG() << "add sock:" << spEventHandler->getFd();
	auto iter = mapHandler_.find(spEventHandler->getFd());
	if (iter == mapHandler_.end())
	{
		mapHandler_[spEventHandler->getFd()] = spEventHandler;
		upEpoll_->addEvent(spEventHandler->getFd(), spEventHandler->getHandlerType());
	}
	else
	{
		mapHandler_[spEventHandler->getFd()] = spEventHandler;
		upEpoll_->mod(spEventHandler->getFd(), spEventHandler->getHandlerType());
	}
}
void Reactor::remove(int fd)
{
	auto iter = mapHandler_.find(fd);
	if (iter != mapHandler_.end())
	{
		mapHandler_.erase(iter);
		upEpoll_->remove(fd);
	}
}

void Reactor::mod(int fd, Event event)
{
	auto iter = mapHandler_.find(fd);
	if (iter != mapHandler_.end())
	{
		upEpoll_->mod(fd, event);
	}
}
void Reactor::loop(int timeout)
{
	upEpoll_->waitEvent(mapHandler_, timeout);
}