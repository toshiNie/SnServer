#pragma once

#ifndef REACTOR_H_
#define REACTOR_H_

#include"EventHandler.h"
#include"Epoll.h"
class ReadThread;
class Reactor
{
	typedef std::map<int, EventHandlerPtr> HandlerMap;
public:
	Reactor();
	void addHandler(EventHandlerPtr spEventHandler);
	void remove(int fd);
	void mod(int fd, Event event);
	void loop(int timeout);

	std::weak_ptr<ReadThread> wpThisThead_;
private:
	
	HandlerMap mapHandler_;
	NsEpollPtr spNsEpoll_;
	std::mutex mutex_;
};
typedef std::shared_ptr<Reactor> ReactorPtr;

#endif