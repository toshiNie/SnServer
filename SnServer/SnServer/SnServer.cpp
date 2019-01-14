// SnServer.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "Time.h"
#include "AsyncLog.h"
#include "TcpServer.h"
#include "SigProcess.h"
//#include "Connector.h"

int main()
{
	std::cout << NsTime::GetStrTimeStamp() << std::endl;
	AsyncLog& thread = AsyncLog::getInstance();
	registeSig();
	//FILE * file = fopen("/dev/null","w");	
	FILE * file = fopen("/dev/stdout","we");
	thread.addLogFile("default", file);
	//thread.addLogFile("default", "ns.log");
	thread.run();
	//auto spConnect = std::make_shared<ConnectThread>();
	//spConnect->init();
	//spConnect->run();
	TcpServer server;
	server.run();
    return 0;
}

