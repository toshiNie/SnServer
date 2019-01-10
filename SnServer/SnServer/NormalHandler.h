#pragma once
#include"EventHandler.h"
#include"Session.h"


class NomalEventHandler : public EventHandler
{
	enum { READ_SIZE = 1024 };
public:
	using  ConnectSessionType = ConnectSession;
	using  ConnectSessionPtr = std::shared_ptr<ConnectSessionType>;

	//typedef  ConnectSession SessionType;
	//typedef std::shared_ptr<SessionType> ConnectSessionPtr;

public:
	NomalEventHandler(ConnectSessionPtr spConnect, ReactorPtr spReactor);
	void readHandler() override;
	void writeHandler() override;
	void errorHandler() override;
	int getFd() override;
protected:
	virtual void onRead();  //判断是不是完整包
	virtual void onMessage();  //完整包处理
	virtual bool onWrite(int len); //写sock判断是否写完
protected:
	ConnectSessionPtr spConnect_;
	ReactorPtr spReactor_;
};
