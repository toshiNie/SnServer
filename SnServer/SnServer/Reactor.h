#pragma once

#ifndef REACTOR_H_
#define REACTOR_H_

#include"noncopyable.h"
#include"EventHandler.h"
#include"ThreadLocalManager.h"
#include"Epoll.h"


class NoticeEventHandler :public EventHandler
{
public:
	NoticeEventHandler(int fd, std::vector<std::function<void()>>& funcs_, std::mutex& mutex);
	void readHandler() override;
	int getFd() override { return fd_; }
private:
	std::vector<std::function<void()>>& noticeFuncs_;
	int fd_;
	std::mutex& mutex_;
};

class LoopThread;
class Reactor : public noncopyable
{
	using HandlerMap = std::unordered_map<int, EventHandlerPtr>;
public:
	Reactor();
	void addHandler(EventHandlerPtr spEventHandler);
	void remove(int fd);
	void mod(int fd, Event event);
	void loop(int timeout);
	void addNoticeEvent(std::function<void()>&& func);

	std::weak_ptr<ThreadLocalManager> wpThreadLocalManager;
private:
	void notice();
private:
	int eventfd_;
	std::vector<std::function<void()>> noticeEvents_;
	std::mutex noticeMutex_;

	HandlerMap mapHandler_;
	std::unique_ptr<Epoll> upEpoll_;
};
using ReactorPtr = std::shared_ptr<Reactor>;

#endif