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
	AcceptHandler(int fd, ReactorPtr spReactor, std::mutex* mutex = nullptr) :sock_(fd), spReactor_(spReactor), mutex_(mutex)
	{
		setHandlerType(ReadEvent);
	}
	~AcceptHandler()
	{
	}
	virtual void readHandle()
	{
		LOG_INFO("accept handle");
		int sock = ::accept(sock_, nullptr, nullptr);
		LOG_INFO("accept sock: " + std::to_string(sock));
		if (sock < 0)
		{
			return;
		}
		socketutil::make_socket_non_blocking(sock);
		typename HandlerType::ConnectSessionPtr spConnect(new typename HandlerType::ConnectSessionType(sock, spReactor_));
		auto spThisThread = spReactor_->wpThisThead_.lock();
		spThisThread->getTimeWheel().addSock(sock, spConnect->getRefIndex());
		spThisThread->getManager().insert(std::make_pair(sock, spConnect));
		auto spReadHandler = std::make_shared<HandlerType>(spConnect, spReactor_);
		spReadHandler->setHandlerType(ReadEvent);
		spReactor_->addHandler(spReadHandler);
		LOG_INFO("AddReadHandler: " + std::to_string(sock));
	}

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
