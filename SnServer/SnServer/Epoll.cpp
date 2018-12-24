#include"stdafx.h"
#include "EventHandle.h"
#include "LogThread.h"
#include "Epoll.h"



int NsEpoll::WaitEvent(std::map<int, EventHandlerPtr> eventHandles, int timeout)
{

	std::vector<epoll_event> vecEvents(eventSize_);
	int num = epoll_wait(epollFd_, vecEvents.data(), eventSize_, timeout);
	if (num > 0)
	{
		for (int i = 0; i < num; ++i)
		{
			//LOG_INFO("epoll event: " +std::to_string(vecEvents[i].events));
			//LOG_INFO("fd: "+std::to_string(vecEvents[i].data.fd));
			int handle = vecEvents[i].data.fd;
			if (vecEvents[i].events & EPOLLERR)
			{
				LOG_DEBUG("errorEvent");
				eventHandles[handle]->ErrorHandle();
			}
			else
			{
				if ((EPOLLIN | EPOLLPRI | EPOLLRDHUP ) & vecEvents[i].events)
				{
					LOG_DEBUG("readEvent");
					eventHandles[handle]->ReadHandle();
				}
				if ((EPOLLOUT ) & vecEvents[i].events)
				{
					LOG_DEBUG("writeEvent");
					eventHandles[handle]->WriteHandle();
				}
			}
		}
	}
	return num;
}
void NsEpoll::AddEvent(int handle, Event type)
{
	//LOG_INFO(__FUNCTION__);
	epoll_event stEvt;
	bzero(&stEvt, sizeof(stEvt));
	stEvt.data.fd = handle;
	if (type & EventType::ReadEvent)
	{
		LOG_DEBUG("EPOLLIN");
		stEvt.events |= EPOLLIN;
	}
	if (type &EventType::WriteEvent)
	{
		LOG_DEBUG("EPOLLOUT");
		stEvt.events |= EPOLLOUT;
	}
	if (0 != epoll_ctl(epollFd_, EPOLL_CTL_ADD, handle, &stEvt))
	{
		LOG_ERROR("add fail");
	}
	++eventSize_;
}
bool NsEpoll::Remove(int handle)
{
	struct epoll_event stEvt;
	bzero(&stEvt, sizeof(stEvt));
	if (0 != epoll_ctl(epollFd_, EPOLL_CTL_DEL, handle, &stEvt))
	{
		LOG_ERROR("del fail" + std::to_string(errno));
		return false;
	}
	--eventSize_;
	return 0;
}

bool NsEpoll::Mod(int handle, Event event)
{
	//LOG_INFO(__FUNCTION__);
	epoll_event stEvt;
	bzero(&stEvt, sizeof(stEvt));
	stEvt.data.fd = handle;
	if (event & EventType::ReadEvent)
	{
		LOG_DEBUG("Mod EPOLLIN");
		stEvt.events |= EPOLLIN;
	}
	if (event &EventType::WriteEvent)
	{
		LOG_DEBUG("Mod EPOLLOUT");
		stEvt.events |= EPOLLOUT;
	}
	if (0 != epoll_ctl(epollFd_, EPOLL_CTL_MOD, handle, &stEvt))
	{
		LOG_ERROR("epoll_ctl error" + std::to_string(errno));
		return false;
	}
	return true;
}