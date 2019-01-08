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
	std::unique_ptr<NsEpoll> upNsEpoll_;
};
using ReactorPtr = std::shared_ptr<Reactor>;

#endif