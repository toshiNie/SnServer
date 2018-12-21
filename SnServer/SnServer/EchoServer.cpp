#include "stdafx.h"
#include "EchoServer.h"
#include "TcpServer.h"
#include "TimeHandler.h"
#include "ReadThread.h"


void AcceptHandler::ReadHandle()
{
	LOG_INFO("accept handle");
	//std::lock_guard <std::mutex> lg(*mutex_);
	int sock = ::accept(sock_, NULL, NULL);
	LOG_INFO("accept sock: " + std::to_string(sock));
	if (sock < 0)
	{
		return;
	}
	socketutil::make_socket_non_blocking(sock);

	ConnectSessionPtr spConnect(new ConnectSession(sock, spReactor_));

	auto spThisThread = spReactor_-> wpThisThead_.lock();
	spThisThread->getTimeWheel().addSock(spConnect);
	spThisThread->getManager().insert(std::make_pair(sock,spConnect));

	EventHandlerPtr spReadHandler(new NomalEventHandler(spConnect, spReactor_));
	spReadHandler->SetHandlerType(ReadEvent);
	spReactor_->AddHandler(spReadHandler);
	LOG_INFO("AddReadHandler: " + std::to_string(sock));
}

void EchoReadHandler::ReadHandle()
{
	auto spThisThread = spReactor_->wpThisThead_.lock();
	spThisThread->getTimeWheel().resetSock(spThisThread->getManager()[sock_]);
	LOG_INFO("echo read handle");
	int r = 0;
	int len = 0;
	int ret = ::read(sock_, (void*)&len, sizeof(len));
	if (ret == 0)
	{
		LOG_INFO("client disconnect:" + std::to_string(sock_));
		spThisThread->removeClient(sock_);
		return;
	}
	if (ret < 0)
	{
		LOG_INFO("error:" + std::to_string(errno));
		spThisThread->removeClient(sock_);;
		return;
	}
	LOG_INFO("read len:" + std::to_string(len));
	std::vector<char> buffer(len);
	while (r < len)
	{
		r = ::read(sock_, buffer.data() + r, len);
		LOG_INFO("read size:" + std::to_string(r));
		if (r <= 0)
		{
			LOG_INFO("error:" + std::to_string(errno));
			if (errno = EAGAIN)
			{
				return;
			}
			spThisThread->removeClient(sock_);
			return;
		}
	}
	readBuffer_.append(buffer.data(), len);
	EventHandlerPtr spHander(new EchoWriteHandler(sock_, spReactor_, std::move(readBuffer_)));
	spReactor_->AddHandler(spHander);
}


void EchoWriteHandler::WriteHandle()
{
	LOG_INFO(__FUNCTION__);
	int w = 0;
	auto spThisThread = spReactor_->wpThisThead_.lock();
	while (writedBuffer_.size() > 0)
	{
		w = ::write(sock_, (void*)writedBuffer_.getReadbuffer(), writedBuffer_.size());
		LOG_INFO("write size:" + std::to_string(w));
		if (w < 0)
		{
			if (errno == EAGAIN)
			{
				continue;
			}
			else
			{
				LOG_INFO("write error:" + std::to_string(errno));
				spThisThread->removeClient(sock_);
			}
		}
		writedBuffer_.consumHead(w);
	}
	EventHandlerPtr spReadHandler(new EchoReadHandler(sock_, spReactor_));
	spReactor_->AddHandler(spReadHandler);
}

void EchoServer::run()
{
	listenAddress_.strIp_ = "127.0.0.1";
	listenAddress_.port = 4321;
	listtenSocket_.BindAddress(listenAddress_);
	if (!listtenSocket_.Listen(1024))
	{
		LOG_INFO("listen failed");
		return;
	}
	socketutil::make_socket_non_blocking(listtenSocket_.GetSockFd());
	LOG_INFO("listen OK");

	for (int i = 0; i < 3; ++i)
	{
		ReadThreadPtr spReadThread(new ReadThread(listtenSocket_.GetSockFd(),nullptr,&mutex_));
		threads_.insert(std::make_pair(ThreadPtr(new std::thread(std::bind(&ReadThread::run, spReadThread))), spReadThread));
	}

	for (auto& item : threads_)
	{
		item.first->join();
	}
}

void EchoServer::acceptThread()
{
	LOG_INFO("acceptThread start");
	int i = 0;
	while (true)
	{
		int sock = listtenSocket_.Accept();
		LOG_INFO("new client connect: " + std::to_string(sock));
		std::lock_guard<std::mutex> lg(mutex_);
		i = i % subReactors_.size();
		ReactorPtr spReactor = subReactors_[i];
		socketutil::make_socket_non_blocking(sock);
		EventHandlerPtr spEventHandller(new EchoReadHandler(sock, spReactor));
		spReactor->AddHandler(spEventHandller);
		i++;
	}
}

void EchoServer::readThread(int threadIndex)
{
	LOG_INFO("readThread start");
	ReactorPtr spReactor(new Reactor());
	EventHandlerPtr spAcceptHandler(new AcceptHandler(listtenSocket_.GetSockFd(), spReactor, &mutex_));
	spReactor->AddHandler(spAcceptHandler);
	while (true)
	{
		spReactor->Loop(10);
		//std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
}


void EchoServer::timerThread()
{
	ReactorPtr spReactor(new Reactor());
	while (true)
	{
		spReactor->Loop(10);
	}
}