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

