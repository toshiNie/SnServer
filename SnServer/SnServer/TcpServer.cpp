#include "stdafx.h"
#include "LogThread.h"
#include "SnBuffer.h"
#include "TcpServer.h"


NomalEventHandler::NomalEventHandler(ConnectSessionPtr spConnect, ReactorPtr spReactor) :
	spReactor_(spReactor), spConnect_(spConnect), spThread_(spReactor->wpThisThead_.lock())
{
}
void NomalEventHandler::ReadHandle()
{
	LOG_INFO("Readhanlde");
	std::vector<char> buffer(READ_SIZE);
	int r = ::read(spConnect_->getFd(), (void*)buffer.data(), READ_SIZE);
	if (r < 0)
	{
		if (errno == EAGAIN)
		{
			r = 0;
			spReactor_->Mod(spConnect_->getFd(), ReadEvent);
			return;
		}
		LOG_INFO("read error: " + std::to_string(errno));
		spThread_->removeClient(spConnect_->getFd());
	}
	else if (r == 0)
	{
		LOG_INFO("client disconnect: " + std::to_string(spConnect_->getFd()));
		spThread_->removeClient(spConnect_->getFd());
	}
	else
	{
		spConnect_->readbuffer_.append(buffer.data(), r);
		spConnect_->onRead();
		//spReactor_->Mod(spConnect_->getFd(), ReadEvent);
	}
	return;
}

void NomalEventHandler::WriteHandle() 
{
	//if (spConnect_->writebuffer_.size() == 0)
	//{
	//	spReactor_->Mod(spConnect_->getFd(), ReadEvent);
	//	return;
	//}
	LOG_INFO("writeHandle :" + std::to_string(spConnect_->writebuffer_.size()));
	int w = ::write(spConnect_->getFd(), spConnect_->writebuffer_.getReadbuffer(), spConnect_->writebuffer_.size());
	if (w < 0)
	{
		if (errno == EAGAIN)
		{
			return;
		}
	}
	else
	{
		if (spConnect_->onWrite(w))
		{
			spReactor_->Mod(spConnect_->getFd(), ReadEvent);
		}
	}
	LOG_INFO("writeHandle :" + std::to_string(w));
	LOG_INFO("writeHandle :" + std::to_string(spConnect_->writebuffer_.size()));
	//std::this_thread::sleep_for(std::chrono::seconds(1));
}

void NomalEventHandler::ErrorHandle()
{
	spThread_->removeClient(spConnect_->getFd());
}


int NomalEventHandler::GetFd() 
{
	return spConnect_->getFd();
}



TcpServer::TcpServer() :spQueue_(MessageQueuePtr(new MessageQueue())), pollNum_(2), workerNum_(1)
{
}
TcpServer::~TcpServer()
{
}
void TcpServer::run()
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

	for (int i = 0; i < pollNum_; ++i)
	{
		ReadThreadPtr spReadThread(new ReadThread(listtenSocket_.GetSockFd(), spQueue_, &mutex_));
		pollThreads_.insert(std::make_pair(ThreadPtr(new std::thread(std::bind(&ReadThread::run, spReadThread))), spReadThread));
	}
	for (int i = 0; i < workerNum_; ++i)
	{
		WorkerPtr spWorker(new MessageProcessThread(spQueue_));
		workerThreads_.insert(std::make_pair(ThreadPtr(new std::thread(std::bind(&MessageProcessThread::run, spWorker))), spWorker));
	}
}