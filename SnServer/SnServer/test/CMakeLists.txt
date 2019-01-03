cmake_minimum_required(VERSION 2.8)
project(SnServerTest)
set (CMAKE_CXX_COMPILER "g++")
add_definitions(-O0 -g -DSA_RESTART)
add_compile_options(-std=c++14)
link_libraries(pthread)


include_directories(../../include)
find_library(GTEST gtest HINTS ../../lib)
add_executable(${PROJECT_NAME}
HttpParserTest.cpp
main.cpp
)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} PUBLIC ${GTEST})

