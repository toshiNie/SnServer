cmake_minimum_required(VERSION 2.8)
project(SnServer)
add_definitions(-g)
add_compile_options(-std=c++11)
link_libraries(pthread)
add_executable(SnServer
	SnServer.cpp
	stdafx.cpp
	FileUtil.cpp)

