#include "stdafx.h"
#include "AsyncLog.h"
#include "SnBuffer.h"
#include "TcpServer.h"

TcpServer::TcpServer() :spQueue_(std::make_shared<MessageQueue>()), pollNum_(1), workerNum_(1), listenSocket_(false)
{
}
TcpServer::~TcpServer()
{
	//listenSocket_.close();
}
void TcpServer::run()
{
	//listenAddress_.ip = "::/0";
	listenAddress_.ip = "0.0.0.0";
	listenAddress_.port = 4321;

	if (!listenSocket_.bindAddress(listenAddress_))
	{
		LOG_ERROR() << "bind error :" << errno;
	}
	if (!listenSocket_.listen(511))
	{
		LOG_ERROR() << " listen failed: " << errno;
		return;
	}
	socketutil::setNonblocking(listenSocket_.getSockFd());
	LOG_INFO() << "listen OK :" << listenSocket_.getSockFd();
	std::vector<MessageQueuePtr> vecQueue;
	for (int i = 0; i < pollNum_; ++i)
	{
		auto spReadThread = std::make_shared<LoopThread>(listenSocket_.getSockFd());
		vecThreads_.emplace_back(ThreadRAII(std::thread(std::bind(&LoopThread::run, spReadThread)), ThreadRAII::DtorAction::join));
		vecQueue.push_back(spReadThread->getQueue());
	}
	for (int i = 0; i < workerNum_; ++i)
	{
		auto spWorker = std::make_shared<MessageProcessThread>(vecQueue[i%vecQueue.size()]);
		vecThreads_.emplace_back(ThreadRAII(std::thread(std::bind(&MessageProcessThread::run, spWorker)), ThreadRAII::DtorAction::join));
	}
}