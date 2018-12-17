#include"stdafx.h"
#include "LogThread.h"
#include"Reactor.h"

void Reactor::AddHandler(EventHandlerPtr spEventHandler)
{
	//std::lock_guard<std::mutex> lg(mutex_);
	auto iter = mapHandler_.find(spEventHandler->GetFd());
	if (iter == mapHandler_.end())
	{
		spNsEpoll_->AddEvent(spEventHandler->GetFd(), spEventHandler->GetHandlerType());
		mapHandler_[spEventHandler->GetFd()] = spEventHandler;
	}
	else
	{
		spNsEpoll_->Mod(spEventHandler->GetFd(), spEventHandler->GetHandlerType());
	}
}
void Reactor::Remove(int fd)
{
	//std::lock_guard<std::mutex> lg(mutex_);
	LOG_INFO(__FUNCTION__);
	auto iter = mapHandler_.find(fd);
	if (iter != mapHandler_.end())
	{
		mapHandler_.erase(iter);
		spNsEpoll_->Remove(fd);
	}
}

void Reactor::Mod(int fd, Event event)
{
	//std::lock_guard<std::mutex> lg(mutex_);
	auto iter = mapHandler_.find(fd);
	if (iter != mapHandler_.end())
	{
		spNsEpoll_->Mod(fd, event);
	}
}
void Reactor::Loop(int timeout)
{
	spNsEpoll_->WaitEvent(mapHandler_, timeout);
}