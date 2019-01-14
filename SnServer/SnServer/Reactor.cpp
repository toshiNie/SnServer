#include "stdafx.h"
#include "AsyncLog.h"
#include "ReadThread.h"
#include "Reactor.h"
#include "Socket.h"
#include <sys/eventfd.h>

NoticeEventHandler::NoticeEventHandler(int fd, std::vector<std::function<void()>>& funcs_, std::mutex& mutex)
	:fd_(fd),
	noticeFuncs_(funcs_),
	mutex_(mutex)
{
}


void NoticeEventHandler::readHandler()
{
	long long  one;
	int ret = ::read(fd_, &one, sizeof(one));
	if (ret == EAGAIN)
	{
		return;
	}
	else
	{
		std::vector<std::function<void()>> funcs;
		{
			std::lock_guard<std::mutex> lg(mutex_);
			funcs.swap(noticeFuncs_);
		}
		for (auto &func : funcs)
		{
			func();
		}
	}
}



Reactor::Reactor() :
	upEpoll_(new Epoll), 
	eventfd_(::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC))
{
	EventHandlerPtr spHandler = std::make_shared<NoticeEventHandler>(eventfd_, noticeEvents_, noticeMutex_);
	spHandler->setHandlerType(ReadEvent);
	addHandler(spHandler);
}
void Reactor::addHandler(EventHandlerPtr spEventHandler)
{
	LOG_DEBUG() << "add sock:" << spEventHandler->getFd();
	auto iter = mapHandler_.find(spEventHandler->getFd());
	if (iter == mapHandler_.end())
	{
		mapHandler_[spEventHandler->getFd()] = spEventHandler;
		upEpoll_->addEvent(spEventHandler->getFd(), spEventHandler->getHandlerType());
	}
	else
	{
		mapHandler_[spEventHandler->getFd()] = spEventHandler;
		upEpoll_->mod(spEventHandler->getFd(), spEventHandler->getHandlerType());
	}
}
void Reactor::remove(int fd)
{
	auto iter = mapHandler_.find(fd);
	if (iter != mapHandler_.end())
	{
		mapHandler_.erase(iter);
		upEpoll_->remove(fd);
	}
}

void Reactor::mod(int fd, Event event)
{
	auto iter = mapHandler_.find(fd);
	if (iter != mapHandler_.end())
	{
		upEpoll_->mod(fd, event);
	}
}
void Reactor::loop(int timeout)
{
	upEpoll_->waitEvent(mapHandler_, timeout);
}


void Reactor::addNoticeEvent(std::function<void()>&& func)
{
	std::lock_guard<std::mutex> lg(noticeMutex_);
	noticeEvents_.emplace_back(func);
	notice();
}


void Reactor::notice()
{
	long long one = 1;
	int ret = ::write(eventfd_, &one, sizeof(one));
	LOG_INFO() << "notice:" << ret << errno;
}