#pragma once
#include"stdafx.h"
#include"ThreadPool.h"
#include"EventHandle.h"
#include"Reactor.h"
#include"Socket.h"
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