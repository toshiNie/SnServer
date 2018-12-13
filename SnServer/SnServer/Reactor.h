#pragma once
#include"stdafx.h"
#include"EventHandle.h"
#include"Epoll.h"

class Reactor;
typedef std::shared_ptr<Reactor> ReactorPtr;
class Reactor
{
	typedef std::map<int, EventHandlerPtr> HandlerMap;
public:


	Reactor() :spNsEpoll_(NsEpollPtr(new NsEpoll))	
	{

	}
	static ReactorPtr GetReactorPtr()
	{
		static thread_local ReactorPtr spReactor(new Reactor);
		return spReactor;
	}
	void AddHandler(EventHandlerPtr spEventHandler)
	{
		if (mapHandler_.find(spEventHandler->GetFd()) == mapHandler_.end())
		{
			spNsEpoll_->AddEvent(spEventHandler->GetFd(),spEventHandler->GetHandlerType());
			mapHandler_[spEventHandler->GetFd()] = spEventHandler;
		}
		else
		{
			LOG_INFO("fuck");
		}
	}
	void Remove(EventHandlerPtr spEventHandler)
	{
		auto iter = mapHandler_.find(spEventHandler->GetFd());
		if (iter != mapHandler_.end())
		{
			mapHandler_.erase(iter);
			spNsEpoll_->Remove(spEventHandler->GetFd());
		}

	}
	void Loop(int timeout)
	{
		spNsEpoll_->WaitEvent(mapHandler_,timeout);
	}
private:
	HandlerMap mapHandler_;
	NsEpollPtr spNsEpoll_;
};


