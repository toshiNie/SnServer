#pragma once
#include"Socket.h"
#include"SocketUtil.h"
#include"LoopThread.h"
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
	Socket listenSocket_;
	std::vector<ThreadRAII> vecThreads_;
	MessageQueuePtr spQueue_;
	int pollNum_;
	int workerNum_;
};