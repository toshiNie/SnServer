cmake_minimum_required(VERSION 2.8)
project(SnServer)
set (CMAKE_CXX_COMPILER "g++")
add_definitions(-O2 -g -DSA_RESTART)
add_compile_options(-std=c++11)
link_libraries(pthread)
add_executable(SnServer
	SnServer.cpp
	stdafx.cpp
	Reactor.cpp
	Epoll.cpp
	TcpServer.cpp
	SocketUtil.cpp
	SigProcess.cpp
	EchoServer.cpp
	Log.cpp
	LogThread.cpp
	LogFile.cpp
	TimeHandler.cpp
	Global.cpp
	ReadThread.cpp
	Session.cpp
	MessageProcessThread.cpp
	FileUtil.cpp)

