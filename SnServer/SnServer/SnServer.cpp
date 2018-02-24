// SnServer.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "Time.h"
#include "Log.h"

int main()
{
	std::cout << NsTime::GetStrTimeStamp() << std::endl;
	NsLog::Instance().AddLogFile("hello","tg.log");
	NsLog &log = NsLog::Instance();
	log.AddLog("hello","123",3);


    return 0;
}

