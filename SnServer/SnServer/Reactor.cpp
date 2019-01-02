#include "stdafx.h"
#include "AsyncLog.h"
#include"ReadThread.h"
#include "Reactor.h"

Reactor::Reactor() :spNsEpoll_(std::make_shared<NsEpoll>())
{
}
void Reactor::addHandler(EventHandlerPtr spEventHandler)
{
	//std::lock_guard<std::mutex> lg(mutex_);
	LOG_DEBUG("add sock:" + std::to_string(spEventHandler->getFd()));
	for (auto& item : mapHandler_)
	{
		LOG_DEBUG(std::to_string(item.first));
	}
	auto iter = mapHandler_.find(spEventHandler->getFd());
	if (iter == mapHandler_.end())
	{
		mapHandler_[spEventHandler->getFd()] = spEventHandler;
		spNsEpoll_->addEvent(spEventHandler->getFd(), spEventHandler->getHandlerType());
	}
	else
	{
		mapHandler_[spEventHandler->getFd()] = spEventHandler;
		spNsEpoll_->mod(spEventHandler->getFd(), spEventHandler->getHandlerType());
	}
}
void Reactor::remove(int fd)
{
	//std::lock_guard<std::mutex> lg(mutex_);
	LOG_INFO(__FUNCTION__);
	auto iter = mapHandler_.find(fd);
	if (iter != mapHandler_.end())
	{
		mapHandler_.erase(iter);
		spNsEpoll_->remove(fd);
	}
}

void Reactor::mod(int fd, Event event)
{
	//std::lock_guard<std::mutex> lg(mutex_);
	auto iter = mapHandler_.find(fd);
	if (iter != mapHandler_.end())
	{
		spNsEpoll_->mod(fd, event);
	}
}
void Reactor::loop(int timeout)
{
	spNsEpoll_->waitEvent(mapHandler_, timeout);
}