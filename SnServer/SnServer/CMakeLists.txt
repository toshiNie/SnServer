cmake_minimum_required(VERSION 2.8)
project(SnServer)
set (CMAKE_CXX_COMPILER "g++")
add_definitions(-O0 -g -DSA_RESTART)
add_compile_options(-std=c++11)
link_libraries(pthread)
add_executable(SnServer
	stdafx.cpp
	Reactor.cpp
	Epoll.cpp
	TcpServer.cpp
	SocketUtil.cpp
	SigProcess.cpp
  AsyncLog.cpp
  Inifile.cpp
	LogFile.cpp
	TimeHandler.cpp
	Global.cpp
	ReadThread.cpp
	Session.cpp
	MessageProcessThread.cpp
	HttpParser.cpp
  	TimeWheel.cpp
	NormalHandler.cpp
  HttpHandler.cpp
  	TimeEvent.cpp
	SnServer.cpp
	FileUtil.cpp)

