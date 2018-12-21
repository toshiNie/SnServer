// SnServer.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "Time.h"
#include "LogThread.h"
#include "EchoServer.h"
#include "TcpServer.h"
#include "SigProcess.h"

int main()
{
	std::cout << NsTime::GetStrTimeStamp() << std::endl;
	LogThread& thread = LogThread::getInstance();
	registeSig();
	//FILE * file = fopen("/dev/null","w");	
	FILE * file = fopen("/dev/stdout","we");
	thread.addLogFile("info", file);
	//thread.addLogFile("info", "ns.log");
	thread.run();
	TcpServer server;
	server.run();
	thread.join();
    return 0;
}

