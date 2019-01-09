#pragma once
#include"Socket.h"
#include"SocketUtil.h"
#include"ReadThread.h"
#include"TreadRAII.h"
#include"MessageProcessThread.h"

class TcpServer
{
	typedef std::shared_ptr<std::thread> ThreadPtr;
public:
	TcpServer();
	~TcpServer();
	void run();
private:
	Address listenAddress_;
	Socket listtenSocket_;
	//std::map<ThreadRAIIPtr, ReadThreadPtr> pollThreads_;
	//std::map<ThreadRAIIPtr, WorkerPtr> workerThreads_;
	std::vector<ThreadRAII> vecThreads_;
	MessageQueuePtr spQueue_;
	int pollNum_;
	int workerNum_;
};