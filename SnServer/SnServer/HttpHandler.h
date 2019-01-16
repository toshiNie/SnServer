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

class HttpNormalHandler : public NormalEventHandler
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