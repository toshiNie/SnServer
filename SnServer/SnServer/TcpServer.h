#pragma once
#include"EventHandle.h"
#include"Reactor.h"
#include"Socket.h"
#include"SocketUtil.h"
#include"Session.h"
#include"ReadThread.h"
#include"MessageProcessThread.h"

class NomalEventHandler : public EventHandler
{
	enum {READ_SIZE = 1024};
public:
	NomalEventHandler(ConnectSessionPtr spConnect, ReactorPtr spReactor);
	void ReadHandle() override;
	void WriteHandle() override;
	void ErrorHandle() override;
	int GetFd() override;

private:
	ConnectSessionPtr spConnect_;
	ReactorPtr spReactor_;
	ReadThreadPtr spThread_;
};


class TcpServer
{
	typedef std::shared_ptr<std::thread> ThreadPtr;
public:
	TcpServer();
	~TcpServer();
	void run();
	static std::mutex globalMutex_;
private:
	Address listenAddress_;
	Socket listtenSocket_;
	std::map<ThreadPtr, ReadThreadPtr> pollThreads_;
	std::map<ThreadPtr, WorkerPtr> workerThreads_;
	MessageQueuePtr spQueue_;
	std::mutex mutex_;
	int pollNum_;
	int workerNum_;
	
};