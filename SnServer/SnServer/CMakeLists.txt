cmake_minimum_required(VERSION 2.8)
project(SnServer)
set (CMAKE_CXX_COMPILER "g++")
add_definitions(-O0 -g -DSA_RESTART)
add_compile_options(-std=c++14)
link_libraries(pthread)
add_executable(SnServer
	stdafx.cpp
	Reactor.cpp
	Epoll.cpp
	TcpServer.cpp
	SocketUtil.cpp
	SigProcess.cpp
	EchoServer.cpp
	Log.cpp
  AsyncLog.cpp
	LogFile.cpp
	TimeHandler.cpp
	Global.cpp
	ReadThread.cpp
	Session.cpp
	MessageProcessThread.cpp
	HttpServer.cpp
	HttpParser.cpp
  TimeWheel.cpp
  AcceptHandler.cpp
  NormalHandler.cpp
  TimeEvent.cpp
	SnServer.cpp
	FileUtil.cpp)

