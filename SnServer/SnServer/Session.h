#pragma once
#ifndef CONNECTSESSION_H_
#define CONNECTSESSION_H_
#include "stdafx.h"
#include "Reactor.h"
#include "TimeHandler.h"

class ConnectSession
{
public:
	ConnectSession(int sock)
		:sock_(sock),index_(-1)
	{

	}
	void setIndex(int index) 
	{
		index_ = index;
	}
	int getIndex()
	{
		return index_;
	}
	int getFd()
	{
		return sock_;
	}
	void close() 
	{
		::close(sock_);
	}

private:
	int sock_;
	int index_;
};
typedef std::shared_ptr<ConnectSession>  ConnectSessionPtr;

#endif // !CONNECTSESSION_H_