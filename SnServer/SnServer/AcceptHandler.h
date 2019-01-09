#pragma once
#include"EventHandler.h"
#include"SocketUtil.h"
#include"TimeWheel.h"
#include"ReadThread.h"
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
		LOG_INFO() << "accept handle";
		int sock = ::accept(sock_, nullptr, nullptr);
		LOG_INFO() << "accept sock: " << sock;
		if (sock < 0)
		{
			return;
		}
		socketutil::setNonblocking(sock);
		typename HandlerType::ConnectSessionPtr spConnect(new typename HandlerType::ConnectSessionType(sock, spReactor_));
		auto spThisThread = spReactor_->wpThisThead_.lock();
		spThisThread->getTimeWheel().addSock(sock, spConnect->getRefIndex());
		spThisThread->getManager().insert(std::make_pair(sock, spConnect));
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
