#pragma once
#include"stdafx.h"
#include"EventHandle.h"
class NsEpoll
{
public:
	NsEpoll()
	{
		epollFd_ = epoll_create1(0);
	}
	~NsEpoll()
	{
		::close(epollFd_);
	}
	int WaitEvent(std::map<int,EventHandlerPtr> eventHandles, int timeout)
	{
		std::vector<epoll_event> vecEvents(eventSize_);
		int num = epoll_wait(epollFd_,vecEvents.data(), eventSize_,timeout);
		if (num > 0)
		{
			for (int i; i < num; ++i)
			{
				int handle = vecEvents[i].data.fd;
				if (vecEvents[i].events & EPOLLERR)
				{
					eventHandles[handle]->ErrorHandle();
				}
				else
				{
					if ((EPOLLIN | EPOLLPRI | EPOLLRDHUP) & vecEvents[i].events)
					{
						eventHandles[handle]->ReadHandle();
					}
					if (EPOLLOUT & vecEvents[i].events)
					{
						eventHandles[handle]->WriteHandle();
					}
				}

			}
		}
		return num;
	}
	void AddEvent(int handle, Event type)
	{
		epoll_event stEvt;
		stEvt.data.fd = handle;
		if (type & EventType::ReadEvent)
		{
			stEvt.events |= EPOLLIN;
		}
		if (type &EventType::WriteEvent)
		{
			stEvt.events |= EPOLLOUT;
		}
		stEvt.events |= EPOLLET;
		if (0 != epoll_ctl(epollFd_, EPOLL_CTL_ADD, handle, &stEvt))
		{

		}
		++eventSize_;
	}
	bool Remove(int handle)
	{
		struct epoll_event ev;
		if (0 != epoll_ctl(epollFd_, EPOLL_CTL_DEL, handle, &ev))
		{
			return false;
		}
		--eventSize_;
		return 0;
	}
private:
	int eventSize_;;
	int epollFd_;
};
typedef std::shared_ptr<NsEpoll> NsEpollPtr;