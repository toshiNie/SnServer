#pragma once

#ifndef REACTOR_H_
#define REACTOR_H_

#include"EventHandle.h"
#include"Epoll.h"
class Reactor
{
	typedef std::map<int, EventHandlerPtr> HandlerMap;
public:
	Reactor() :spNsEpoll_(NsEpollPtr(new NsEpoll))	
	{
	}
	void AddHandler(EventHandlerPtr spEventHandler);
	void Remove(int fd);
	void Mod(int fd, Event event);
	void Loop(int timeout);

	ReadThreadWeakPtr wpThisThead_;
private:
	HandlerMap mapHandler_;
	NsEpollPtr spNsEpoll_;
	std::mutex mutex_;
};
typedef std::shared_ptr<Reactor> ReactorPtr;

#endif