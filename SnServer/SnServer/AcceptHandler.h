#pragma once
#include"EventHandler.h"
#include"SocketUtil.h"
#include"TimeWheel.h"
#include"LoopThread.h"
#include"Reactor.h"

template<typename HandlerType>
class AcceptHandler : public EventHandler
{
public:
	AcceptHandler(int fd, ReactorPtr spReactor):sock_(fd), spReactor_(spReactor)
	{
		setHandlerType(ReadEvent);
	}
	~AcceptHandler()
	{
	}
	virtual void readHandler() override
	{
		int sock = ::accept(sock_, nullptr, nullptr);
		
		if (sock < 0)
		{
			LOG_ERROR() << "accept error: " << errno << strerror(errno);
			return;
		}
		LOG_INFO() << "accept sock: " << sock;
		socketutil::setNonblocking(sock);
		typename HandlerType::ConnectSessionPtr spConnect(new typename HandlerType::ConnectSessionType(sock, spReactor_));
		auto spThisThread = spReactor_->wpThreadLocalManager.lock();
		spThisThread->addConnection(spConnect);
		auto spReadHandler = std::make_shared<HandlerType>(spConnect, spReactor_);
		spReadHandler->setHandlerType(ReadEvent);
		spReactor_->addHandler(spReadHandler);
		LOG_INFO() << "AddReadHandler: " << sock;
	}
	virtual void errorHandler() override
	{
		LOG_ERROR() << "error";
		spReactor_->remove(sock_);
		::close(sock_);
	}
	virtual int getFd() override
	{ return sock_; }
private:
	int sock_;
	ReactorPtr spReactor_;
};
