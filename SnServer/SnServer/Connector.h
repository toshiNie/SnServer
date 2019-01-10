#pragma once
#include"stdafx.h"
#include"AsyncLog.h"
#include"Reactor.h"
#include"Socket.h"
#include"Session.h"
#include"EventHandler.h"
#include"ThreadLocalManager.h"
#include"NormalHandler.h"

template <typename HandlerType>
class ConnectHandler : public EventHandler
{
public:
	ConnectHandler(int sock, ReactorPtr spReactor) : sock_(sock), spReactor_(spReactor) {}
	void readHandler() override
	{
		LOG_INFO() << "read";
	}
	void writeHandler() override
	{
		typedef typename HandlerType::ConnectSessionPtr ConnectSessionPtr;
		typedef typename HandlerType::ConnectSessionType ConnectSessionType;
		ConnectSessionPtr spConnection = std::make_shared<ConnectSessionType>(sock_, spReactor_);
		char a[] = "helloworld";
		int size = sizeof(a);
		spConnection->writeBuffer.append((const char*)&size, sizeof(int));
		spConnection->writeBuffer.append(a, size);
		EventHandlerPtr spEvent = std::make_shared<HandlerType>(spConnection, spReactor_);
		spEvent->setHandlerType(WriteEvent|ReadEvent);
		spReactor_->addHandler(spEvent);
	}
	void errorHandler() override
	{
		LOG_INFO() << "error";
	}
	int getFd() override
	{
		return sock_;
	}
private:
	int sock_;
	ReactorPtr spReactor_;
};

class ConnectThread: public ThreadLocalManager
{
public:
	ConnectThread() : ThreadLocalManager(),spReactor_(std::make_shared<Reactor>())
	{}
	void init()
	{
		spReactor_->wpThreadLocalManager = shared_from_this();
		Address address;
		address.strIp_ = "192.168.88.132";
		address.port = 4321;
		connectConf_.push_back(std::make_pair(address, 1));
		Socket sock;
		sock.setNonblock();
		sock.connect(address);
		EventHandlerPtr spEvent = std::make_shared<ConnectHandler<NomalEventHandler> >(sock.GetSockFd(), spReactor_);
		spEvent->setHandlerType(WriteEvent);
		spReactor_->addHandler(spEvent);
	}
	void run()
	{
		while (true)
		{
			spReactor_->loop(10);
		}
	}
	virtual void addConnection(std::shared_ptr<ConnectSession> spConnect) {};
	virtual void removeConnection(std::shared_ptr<ConnectSession> spConnect) 
	{ 
		spReactor_->remove(spConnect->getFd());
		spConnect->close(); 
	};
	virtual void resetConnection(std::shared_ptr<ConnectSession> spConnect) {};
	virtual void pushToQueue(std::shared_ptr<MessagePackage> spMessage) {};

private:
	ReactorPtr spReactor_;
	size_t connectionNum_;
	std::map<int, std::shared_ptr<ConnectSession>> connnectSessions_;
	std::vector<std::pair<Address, int>> connectConf_;
};
