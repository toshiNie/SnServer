#pragma once
#include"EventHandler.h"
#include"Session.h"


class NomalEventHandler : public EventHandler
{
	enum { READ_SIZE = 1024 };
public:
	NomalEventHandler(ConnectSessionPtr spConnect, ReactorPtr spReactor);
	void readHandle() override;
	void writeHandle() override;
	void errorHandle() override;
	int getFd() override;
private:
	virtual void onRead();
	virtual void onMessage(std::vector<char>& buffer);
	virtual bool onWrite(int len);
private:
	ConnectSessionPtr spConnect_;
	ReactorPtr spReactor_;
	std::shared_ptr<ReadThread> spThread_;
};
