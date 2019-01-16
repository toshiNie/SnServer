#pragma once
#include"stdafx.h"
#include"AsyncLog.h"
#include"Reactor.h"
#include"Socket.h"
#include"Session.h"
#include"EventHandler.h"
#include"ThreadLocalManager.h"
#include"TimeHandler.h"
#include"NormalHandler.h"

template <typename HandlerType>
class ConnectHandler : public EventHandler
{
public:
	ConnectHandler(int sock, ReactorPtr spReactor)
		: sock_(sock), 
		spReactor_(spReactor)
	{}
	void readHandler() override
	{
		LOG_INFO() << "read";
	}
	void writeHandler() override
	{
		typedef typename HandlerType::ConnectSessionPtr ConnectSessionPtr;
		typedef typename HandlerType::ConnectSessionType ConnectSessionType;
		ConnectSessionPtr spConnection = std::make_shared<ConnectSessionType>(sock_, spReactor_);
		spReactor_->wpThreadLocalManager.lock()->addConnection(spConnection);
		EventHandlerPtr spEvent = std::make_shared<HandlerType>(spConnection, spReactor_);
		spEvent->setHandlerType(ReadEvent);
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
private:
	int sock_;
	ReactorPtr spReactor_;
};
class Connecter: public ThreadLocalManager
{
public:
	Connecter(const std::string& ip, int port, size_t num, ReactorPtr spReactor)
		:address_(ip, port),
		connectNum_(num),
		spReactor_(spReactor)
	{

	}
	void connect()
	{
		for (size_t i = 0; i < connectNum_; ++i)
		{
			Socket sock;
			sock.connect(address_);
			sock.setNonblock();
			EventHandlerPtr spEvent = std::make_shared<ConnectHandler<NormalEventHandler> >(sock.getSockFd(), spReactor_, std::bind(&Connecter::addConnection, this, std::placeholders::_1));
			spEvent->setHandlerType(WriteEvent);
			spReactor_->addHandler(spEvent);
		}
	}
	void addConnection(std::shared_ptr<ConnectSession> spConnect)
	{
		connectManager_.insert(std::make_pair(spConnect->getFd(), spConnect));
	}

private:
	Address address_;
	size_t connectNum_;
	ReactorPtr spReactor_;
	std::unordered_map<int, std::shared_ptr<ConnectSession>> connectManager_;
	
};

class ConnectThread: public ThreadLocalManager 
{
public:
	ConnectThread() : ThreadLocalManager(),spReactor_(std::make_shared<Reactor>())
	{}
	void init()
	{
		//spReactor_->wpThreadLocalManager = shared_from_this();
		//Address address;
		//address.ip = "192.168.88.132";
		//address.port = 4321;
		//connectConf_.push_back(std::make_pair(address, 1));
		//Socket sock;
		//sock.setNonblock();
		//sock.connect(address);
		//EventHandlerPtr spEvent = std::make_shared<ConnectHandler<NormalEventHandler> >(sock.GetSockFd(), spReactor_);
		//spEvent->setHandlerType(WriteEvent);
		//spReactor_->addHandler(spEvent);
		//auto spTimeTEvent = std::make_shared<TimeEvent>(std::bind(&ConnectThread::sendHeartBeat, this));
		//spTimeTEvent->setTime(10000);
		//EventHandlerPtr spTimeHandler = std::make_shared<TimeHandler>(spTimeTEvent, spReactor_);
		//spReactor_->addHandler(spTimeHandler);
	}
	void run()
	{
		while (true)
		{
			spReactor_->loop(10);
		}
	}

	void sendHeartBeat()
	{
		for (auto& item : connnectSessions_)
		{
			char heartbeat[] = "helloworld";
			item.second->write(heartbeat, sizeof(heartbeat));
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
	std::map<int, std::shared_ptr<ConnectSession> > connnectSessions_;
	std::vector<std::pair<Address, int>> connectConf_;
};
