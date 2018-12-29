#pragma once
#include"EventHandler.h"
#include"HttpRequst.h"
#include"Message.h"



class HttpMessagePackage :public MessagePackage
{
public:
	~HttpMessagePackage()
	{
	}
	HttpRequest httpRequset;
};
using HttpMessagePackagePtr = std::shared_ptr<HttpMessagePackage>;



class HttpConnectSession : public ConnectSession
{
public:
	HttpConnectSession(int sock, ReactorPtr spReactor)
		:ConnectSession(sock, spReactor),
		contentLength_(-1)
	{

	}

public:
	int contentLength_;
	HttpRequest httpRequset_;
};
using HttpConnectSessionPtr = std::shared_ptr<HttpConnectSession>;

class HttpHandler : public EventHandler
{
	enum { READ_SIZE  = 4096 };
public:
	HttpHandler(HttpConnectSessionPtr spConnect, ReactorPtr spReactor);

	void readHandle() override;

	void writeHandle() override;

	int getFd() override
	{
		return spConnect_->getFd();
	}

private:
	void onRead();

	void onMessage();

	bool onWrite(int len);

private:
	HttpConnectSessionPtr spConnect_;
	ReactorPtr spReactor_;
	std::shared_ptr<ReadThread> spThread_;
};