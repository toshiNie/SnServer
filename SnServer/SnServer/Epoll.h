#pragma once
#ifndef NSEPOLL_H_
#define NSEPOLL_H_
class Epoll
{
public:
	Epoll():eventSize_(0)
	{
		epollFd_ = epoll_create1(0);
	}
	~Epoll()
	{
		::close(epollFd_);
	}
	int waitEvent(std::map<int, EventHandlerPtr>& eventHandles, int timeout);
	void addEvent(int handle, Event type);
	bool remove(int handle);
	bool mod(int handle, Event event);
private:
	int eventSize_;
	int epollFd_;
};
#endif