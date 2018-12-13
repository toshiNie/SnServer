#pragma once
#include"stdafx.h"
#include"EventHandle.h"
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
	int WaitEvent(std::map<int,EventHandlerPtr> eventHandles, int timeout)
	{
		
		std::vector<epoll_event> vecEvents(eventSize_);
		int num = epoll_wait(epollFd_,vecEvents.data(), eventSize_,timeout);
		//LOG_INFO(__FUNCTION__ + std::to_string(num)+ "  " + std::to_string(eventHandles.size()));
		if (num > 0)
		{
			for (int i = 0; i < num; ++i)
			{
				int handle = vecEvents[i].data.fd;
				if (vecEvents[i].events & EPOLLERR)
				{
					LOG_INFO("errorEvent");
					eventHandles[handle]->ErrorHandle();
				}
				else
				{
					if ((EPOLLIN | EPOLLPRI | EPOLLRDHUP) & vecEvents[i].events)
					{
						LOG_INFO("readEvent");
						eventHandles[handle]->ReadHandle();
					}
					if (EPOLLOUT & vecEvents[i].events)
					{
						LOG_INFO("writeEvent");
						eventHandles[handle]->WriteHandle();
					}
				}
			}
		}
		return num;
	}
	void AddEvent(int handle, Event type)
	{
		LOG_INFO(__FUNCTION__);
		epoll_event stEvt;
		stEvt.data.fd = handle;
		if (type & EventType::ReadEvent)
		{
			LOG_INFO("EPOLLIN");
			stEvt.events |= EPOLLIN;
		}
		if (type &EventType::WriteEvent)
		{
			LOG_INFO("EPOLLOUT");
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