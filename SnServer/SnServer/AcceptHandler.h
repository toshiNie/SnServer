#pragma once
#include"EventHandler.h"
#include"Reactor.h"
class AcceptHandler : public EventHandler
{
public:
	AcceptHandler(int fd, ReactorPtr spReactor, std::mutex* mutex = nullptr) :sock_(fd), spReactor_(spReactor), mutex_(mutex)
	{
		setHandlerType(ReadEvent);
	}
	~AcceptHandler()
	{
	}
	virtual void readHandle();
	virtual void errorHandle()
	{
		LOG_ERROR("error");
		spReactor_->remove(sock_);
		::close(sock_);
	};
	virtual int getFd() { return sock_; }
private:
	int sock_;
	ReactorPtr spReactor_;
	std::mutex *mutex_;
};
