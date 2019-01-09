#pragma once
#include"EventHandler.h"
#include"HttpRequst.h"
#include"NormalHandler.h"
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

class HttpHandler
	: public EventHandler
{
	enum { READ_SIZE  = 1024 };
public:
	using ConnectSessionType = HttpConnectSession;
	using ConnectSessionPtr = std::shared_ptr<ConnectSessionType>;

	//typedef  HttpConnectSession SessionType;
	//typedef std::shared_ptr<SessionType> ConnectSessionPtr;
public:
	HttpHandler(ConnectSessionPtr spConnect, ReactorPtr spReactor);

	void readHandler() override;

	void writeHandler() override;

	void errorHandler() override;

	int getFd() override
	{
		return spConnect_->getFd();
	}
private:
	void onRead();

	void onMessage();

	bool onWrite(int len);

private:
	ConnectSessionPtr spConnect_;
	ReactorPtr spReactor_;
	std::shared_ptr<ReadThread> spThread_;
};



class HttpNormalHandler : public NomalEventHandler
{
public:
	using  ConnectSessionType = HttpConnectSession;
	using ConnectSessionPtr = std::shared_ptr<ConnectSessionType>;
public:
	HttpNormalHandler(ConnectSessionPtr spConnect, ReactorPtr spReactor);
private:
	void onRead() override; 
	void onMessage() override;
	bool onWrite(int len) override;
private:
	ConnectSessionPtr spHttpConnect_;
};