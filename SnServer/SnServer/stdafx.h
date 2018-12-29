// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once
#ifndef STDAFX_H_
#define STDAFX_H_

#include<iostream>
#include<sstream>
#include<string>
#include<map>
#include<set>
#include<vector>
#include<memory>
#include<functional>
#include<chrono>
#include<ctime>
#include<iomanip>
#include<thread>
#include<mutex> 
#include<queue>
#include<list>
#include<condition_variable>
#include<cctype>

//#ifdef WIN32
//#include<winsock.h>
//#else
#include <unistd.h>
#include <netdb.h>  
#include <fcntl.h> 
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>  
#include <netdb.h>  
#include <sys/epoll.h>  
#include <arpa/inet.h>
#include <sys/stat.h>
//#endif // !WIN32
#include <stdio.h>

#endif