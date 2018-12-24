#pragma once
#ifndef ECHOSERVER_H_
#define ECHOSERVER_H_
#include"Reactor.h"
#include"Socket.h"
#include"SocketUtil.h"
#include"SnBuffer.h"
#include"LogThread.h"
#include"EventHandle.h"
#include "Session.h"
#include "TimeWheel.h"
#include "ReadThread.h"
	
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
	virtual void WriteHandle();
	virtual void ErrorHandle()
	{
		LOG_ERROR("error");
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
	EchoReadHandler(int fd, ReactorPtr spReactor) :sock_(fd), readBuffer_(1024), spReactor_(spReactor)
	{
		SetHandlerType(ReadEvent);
	}
	~EchoReadHandler()
	{
	}
	virtual void ReadHandle();
	virtual void ErrorHandle()
	{
		LOG_ERROR("error");
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
	AcceptHandler(int fd, ReactorPtr spReactor, std::mutex* mutex  = NULL) :sock_(fd), spReactor_(spReactor), mutex_(mutex)
	{
		SetHandlerType(ReadEvent);
	}
	~AcceptHandler()
	{
	}
	virtual void ReadHandle();
	virtual void ErrorHandle()
	{
		LOG_ERROR("error");
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
	void readThread(int treadIndex);
	void timerThread();
private:
	Address listenAddress_;
	Socket listtenSocket_;
	std::map<ThreadPtr, ReadThreadPtr> threads_;
	std::vector<ReactorPtr> subReactors_;
	std::mutex mutex_;
};

#endif