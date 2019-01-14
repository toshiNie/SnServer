#include"stdafx.h"
#include"AsyncLog.h"
#include"SnBuffer.h"
#include"Reactor.h"
#include"Socket.h"
#include"SocketUtil.h"
#include"Session.h"
#include"ReadThread.h"
#include "Session.h"
#include"HttpParser.h"
#include"HttpHandler.h"



HttpNormalHandler::HttpNormalHandler(ConnectSessionPtr spConnect, ReactorPtr spReactor) :NomalEventHandler(spConnect, spReactor), spHttpConnect_(spConnect)
{

}

void HttpNormalHandler::onRead()
{
	const char * buffer = spConnect_->readBuffer.getReadbuffer();
	LOG_DEBUG() << buffer;
	const char* index = ::strstr(buffer, "\r\n");
	const char* index1 = ::strstr(buffer, "\r\n\r\n");

	HttpRequest *req = &(spHttpConnect_->httpRequset_);
	if (index1 != nullptr)
	{
		if (spHttpConnect_->contentLength_ < 0)
		{
			HttpParser::parseHttpFirstline(buffer, index - buffer, req);
			if (HttpParser::parseHttpHeader(buffer + (index - buffer) + 2, index1 - buffer + 4, req))
			{
				spHttpConnect_->contentLength_ = req->getContentLength();
			}
		}

		if (req->getContentLength() + (index1 - buffer) <= spConnect_->readBuffer.size())
		{

			spConnect_->readBuffer.consumHead(index1 - buffer + 4);
			if (spHttpConnect_->contentLength_ > 0)
			{
				auto& content = req->getContent();
				content.resize(req->getContentLength() + 1);
				spConnect_->readBuffer.read(content, req->getContentLength());
				onMessage();
			}
			else if (spHttpConnect_->contentLength_ == 0)
			{
				spConnect_->readBuffer.consumHead(index1 - buffer + 4);
				onMessage();
			}
		}
	}
	return;
}
void HttpNormalHandler::onMessage()
{
	MessagePackagePtr spMessage;
	auto spHttpMessage = std::make_shared<HttpMessagePackage>();
	spHttpMessage->spConnect = spConnect_;
	spHttpMessage->httpRequset.swap(spHttpConnect_->httpRequset_);
	spMessage = spHttpMessage;
	spHttpConnect_->contentLength_ = -1;
	spReactor_->wpThreadLocalManager.lock()->pushToQueue(spMessage);
}
bool HttpNormalHandler::onWrite(int len)
{
	if (len < spConnect_->writeBuffer.size())
	{
		spConnect_->writeBuffer.consumHead(len);
		return false;
	}
	else
	{
		spConnect_->writeBuffer.reset();
		return true;
	}
}
