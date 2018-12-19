#pragma once
#include"ThreadPool.h"
#include"EventHandle.h"
#include"Reactor.h"
#include"Socket.h"
#include"SocketUtil.h"
#include"SnBuffer.h"
#include"LogThread.h"
#include<map>
class NsTcpServer
{
public:
	NsTcpServer() {}
	~NsTcpServer()
	{

	}
private:
	Address addrLocol_;
	CThreadPool pool_;

};

class EchoReadHandler;
class EchoWriteHandler;

class EchoWriteHandler :public EventHandler
{
public:
	EchoWriteHandler(int fd, ReactorPtr spReactor, SnBuffer&& buffer) :sock_(fd), spReactor_(spReactor), writedBuffer_(std::move(buffer))
	{
		SetHandlerType(WriteEvent);
	}
	~EchoWriteHandler()
	{
	}
	virtual void ReadHandle()
	{
		LOG_INFO("w-r");
	}
	virtual void WriteHandle();

	virtual void ErrorHandle()
	{
		LOG_INFO("error");
		spReactor_->Remove(sock_);
		::close(sock_);
	};
	virtual int GetFd() { return sock_; }

private:
	SnBuffer writedBuffer_;
	int sock_;
	ReactorPtr spReactor_;
};


class EchoReadHandler :public EventHandler
{
public:
	EchoReadHandler(int fd, ReactorPtr spReactor):sock_(fd),readBuffer_(1024), spReactor_(spReactor)
	{
		SetHandlerType(ReadEvent);
	}
	~EchoReadHandler()
	{
	}
	virtual void ReadHandle();
	virtual void WriteHandle() { LOG_INFO("WRITE"); };
	virtual void ErrorHandle() 
	{
		LOG_INFO("error");
		spReactor_->Remove(sock_);
		::close(sock_);
	};
	virtual int GetFd() { return sock_; }
private:
	int readSize()
	{
		int len = 0;
		int size = read(sock_, (void*)&len, sizeof(len));
		return len;
	}
private:
	SnBuffer readBuffer_;
	int sock_;
	ReactorPtr spReactor_;
};


class AcceptHandler : public EventHandler
{
public:
	AcceptHandler(int fd, ReactorPtr spReactor, std::mutex* mutex) :sock_(fd), spReactor_(spReactor),mutex_(mutex)
	{
		SetHandlerType(ReadEvent);
	}
	~AcceptHandler()
	{
	}
	virtual void ReadHandle();
	virtual void WriteHandle() { LOG_INFO("WRITE"); };
	virtual void ErrorHandle()
	{
		LOG_INFO("error");
		spReactor_->Remove(sock_);
		::close(sock_);
	};
	virtual int GetFd() { return sock_; }
private:
	int sock_;
	ReactorPtr spReactor_;
	std::mutex *mutex_;
};


class EchoServer
{
	typedef std::shared_ptr<std::thread> ThreadPtr;
public:
	EchoServer()
	{
	}
	~EchoServer()
	{
	}
	
	static EchoServer& getInstance()
	{
		static EchoServer echo;
		return echo;
	}

	void run();
	void acceptThread();
	void readThread();

private:
	Address listenAddress_;
	Socket listtenSocket_;
	CThreadPool pool_;
	std::vector<ThreadPtr> threads_;
	std::vector<ReactorPtr> subReactors_;
	std::mutex mutex_;
};