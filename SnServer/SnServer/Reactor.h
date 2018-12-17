#pragma once

#ifndef REACTOR_H_
#define REACTOR_H_

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
	void AddHandler(EventHandlerPtr spEventHandler);
	void Remove(int fd);
	void Mod(int fd, Event event);
	void Loop(int timeout);

private:
	HandlerMap mapHandler_;
	NsEpollPtr spNsEpoll_;
	std::mutex mutex_;
};

#endif