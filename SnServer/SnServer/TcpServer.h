#pragma once
#include"stdafx.h"
#include"ThreadPool.h"
#include"EventHandle.h"
#include"Reactor.h"
#include"Socket.h"
#include"SocketUtil.h"
#include"SnBuffer.h"
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
	EchoWriteHandler(int fd, SnBuffer& buffer) :sock_(fd), writedBuffer_(buffer)
	{

	}
	~EchoWriteHandler()
	{

	}
	virtual void ReadHandle()
	{

	}
	virtual void WriteHandle()
	{
		std::cout << "WriteHandle" << std::endl;
		socketutil::writen(sock_, (void*)writedBuffer_.getbuffer(), writedBuffer_.size());
		writedBuffer_.reset();
	}
	virtual void ErrorHandle()
	{
		LOG_INFO("error");
		::close(sock_);
		spReactor_->Remove(sock_);
	};
	virtual int GetFd() { return sock_; }

private:
	SnBuffer& writedBuffer_;
	int sock_;
	ReactorPtr spReactor_;
};


class EchoReadHandler :public EventHandler
{
public:
	EchoReadHandler(int fd, ReactorPtr spReactor):sock_(fd),readBuffer_(1024), spReactor_(spReactor)
	{

	}
	~EchoReadHandler()
	{
	}
	virtual void ReadHandle()
	{
		LOG_INFO(__FUNCTION__);
		int r = 0;
		while (true)
		{
			LOG_INFO("Loop");
			std::vector<char> buffer(1025);
			r = socketutil::readline(sock_, buffer.data(), 1024);
			if (r < 0)
			{
				std::cout << "error:" << errno;
				::close(sock_);
				spReactor_->Remove(sock_);
				return;
			}
			if (r == 0)
			{
				LOG_INFO("client disconnect");
				readBuffer_.reset();
				::close(sock_);
				spReactor_->Remove(sock_);
				return;
			}
			std::cout << "read size:" << r << std::endl;
			if (r < 1024)
			{
				readBuffer_.append(buffer.data(), r);
				break;
			}
			readBuffer_.append(buffer.data(), r - 1);
		}
		std::cout << readBuffer_.getbuffer() << std::endl;
		//int w = 0;
		//int w = socketutil::writen(sock_, (void*)readBuffer_.getbuffer(), readBuffer_.size());
		//std::cout << "write size:" << w << std::endl;
		//if (w < r)
		//{
		//	readBuffer_.consumHead(w);
		//	EventHandlerPtr spHander(new EchoWriteHandler(sock_, readBuffer_));
		//	spHander->SetHandlerType(WriteEvent);
		//	ReactorPtr spReactor = Reactor::GetReactorPtr();
		//	spReactor->AddHandler(spHander);
		//}
		//else
		//{
		//	readBuffer_.reset();
		//	spReactor_->Mod(sock_, ReadEvent);
		//}
		//spReactor_->Mod(sock_, ReadEvent);
		readBuffer_.reset();
	}
	virtual void WriteHandle() { LOG_INFO("WRITE"); };
	virtual void ErrorHandle() 
	{
		LOG_INFO("error");
		::close(sock_);
		spReactor_->Remove(sock_);
	};
	virtual int GetFd() { return sock_; }
	
private:
	SnBuffer readBuffer_;
	int sock_;
	ReactorPtr spReactor_;

};


class EchoServer
{
public:
	EchoServer():spReactor_(Reactor::GetReactorPtr()) 
	{}
	
	~EchoServer()
	{
	}
	void run()
	{
		listenAddress_.strIp_ = "127.0.0.1";
		listenAddress_.port = 4321;
		listtenSocket_.BindAddress(listenAddress_);
		listtenSocket_.Listen(1024);
		LOG_INFO("listen OK");

		for (int i = 0; i < 5; ++i)
		{
			pool_.PushPack(std::bind(&EchoServer::readThread, this));
		}
		pool_.PushPack(std::bind(&EchoServer::acceptThread,this));
		pool_.Start();

		while (pool_.isRunning())
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		}
	}

	void acceptThread()
	{
		LOG_INFO("acceptThread start");
		int i = 0;
		while (true)
		{
			int sock = listtenSocket_.Accept();
			LOG_INFO("new fd" + std::to_string(sock));
			i = i % subReactors_.size();
			socketutil::make_socket_non_blocking(sock);
			EventHandlerPtr spEventHandller(new EchoReadHandler(sock, subReactors_[i]));
			spEventHandller->SetHandlerType(ReadEvent);
			subReactors_[i]->AddHandler(spEventHandller);
			i++;
		}
	}

	void readThread()
	{
		LOG_INFO("readThread start");
		ReactorPtr spReactor(new Reactor());
		subReactors_.push_back(spReactor);
		while (true)
		{
			spReactor->Loop(10);
		}
	}

private:
	ReactorPtr spReactor_;
	Address listenAddress_;
	Socket listtenSocket_;
	CThreadPool pool_;
	std::vector<ReactorPtr> subReactors_;
};