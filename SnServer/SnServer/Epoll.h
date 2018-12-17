#pragma once
#ifndef NSEPOLL_H_
#define NSEPOLL_H_
class NsEpoll
{
public:
	NsEpoll():eventSize_(0)
	{
		epollFd_ = epoll_create1(0);
	}
	~NsEpoll()
	{
		::close(epollFd_);
	}
	int WaitEvent(std::map<int, EventHandlerPtr> eventHandles, int timeout);
	void AddEvent(int handle, Event type);
	bool Remove(int handle);
	bool Mod(int handle, Event event);
private:
	int eventSize_;;
	int epollFd_;
};
typedef std::shared_ptr<NsEpoll> NsEpollPtr;
#endif