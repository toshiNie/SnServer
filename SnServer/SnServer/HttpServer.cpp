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
#include"HttpServer.h"


HttpHandler::HttpHandler(ConnectSessionPtr spConnect, ReactorPtr spReactor)
	:spConnect_(spConnect),
	spReactor_(spReactor),
	spThread_(spReactor_->wpThisThead_.lock())
{
	LOG_DEBUG("HttpHandler" + std::to_string(spConnect->getFd()));
}
void HttpHandler::readHandle()
{
	LOG_DEBUG("Readhanlde");
	spThread_->getTimeWheel().resetSock(spConnect_->getFd(), spConnect_->getRefIndex());
	//std::vector<char> buffer(READ_SIZE);
	char buffer[READ_SIZE] = {0};
	//int r = ::read(spConnect_->getFd(), (void*)buffer.data(), READ_SIZE);
	int remainLen = spConnect_->readBuffer.getRemainSize();
	int r = socketutil::readv2(spConnect_->getFd(), spConnect_->readBuffer.getRemainbuffer(), remainLen, buffer, sizeof(buffer));
	LOG_INFO("remain size" + std::to_string(remainLen));
	LOG_INFO("read size" + std::to_string(r));
	if (r < 0)
	{
		if (errno == EAGAIN)
		{
			return;
		}
		LOG_ERROR("read error: " + std::to_string(errno));
		spThread_->removeClient(spConnect_->getFd());
	}
	else if (r == 0)
	{
		LOG_INFO("client disconnect: " + std::to_string(spConnect_->getFd()));
		spThread_->removeClient(spConnect_->getFd());
	}
	else
	{
		if (r > remainLen)
		{
			spConnect_->readBuffer.peek(remainLen);
			spConnect_->readBuffer.append(buffer,r - remainLen);
		}
		else
		{
			spConnect_->readBuffer.peek(r);
		}
		onRead();
	}
	return;
}
void HttpHandler::writeHandle()
{
	LOG_DEBUG("TRYLOCK");
	std::lock_guard<std::mutex> lg(spConnect_->writeMutex);
	spThread_->getTimeWheel().resetSock(spConnect_->getFd(),spConnect_->getRefIndex());
	LOG_DEBUG("GETLOCK");
	int w = ::write(spConnect_->getFd(), spConnect_->writeBuffer.getReadbuffer(), spConnect_->writeBuffer.size());
	LOG_INFO("writebuffer size: " + std::to_string(spConnect_->writeBuffer.size()));
	LOG_INFO("writen size: " + std::to_string(w));
	if (w < 0)
	{
		if (errno == EAGAIN)
		{
			return;
		}
		LOG_ERROR("write error: " + std::to_string(errno));
	}
	else
	{
		if (onWrite(w))
		{
			spReactor_->mod(spConnect_->getFd(), ReadEvent);
		}
	}
	LOG_DEBUG("REALISELOCK");
}
void HttpHandler::onRead()
{
	const char * buffer = spConnect_->readBuffer.getReadbuffer();
	//LOG_INFO(buffer);
	const char* index = ::strstr(buffer, "\r\n");
	const char* index1 = ::strstr(buffer, "\r\n\r\n");

	HttpRequest *req = &(spConnect_->httpRequset_);
	if (index1 != nullptr)
	{
		if (spConnect_->contentLength_ < 0)
		{
			HttpParser::parseHttpFirstline(buffer, index - buffer, req);
			HttpParser::parseHttpHeader(buffer + (index - buffer) + 2, index1 - buffer + 4, req);
			spConnect_->contentLength_ = req->getContentLength();
			
		}
		//LOG_INFO("0." + std::to_string(spConnect_->contentLength_));

		if (req->getContentLength() + (index1 - buffer) <= spConnect_->readBuffer.size())
		{
			
			std::vector<char> buf(index1 - buffer + 4);
			//LOG_INFO("1." + std::to_string(buf.size()) + ":" + std::to_string(spConnect_->readBuffer.size()));
			spConnect_->readBuffer.read(buf, buf.size());
			//LOG_INFO("2." + std::to_string(buf.size()) + ":" + std::to_string(spConnect_->readBuffer.size()));
			if (req->getContentLength() > 0)
			{
				auto& content = req->getContent();
				content.reserve(req->getContentLength());
				//LOG_INFO("3." + std::to_string(req->content_.size()) + ":" + std::to_string(spConnect_->readBuffer.size()));
				spConnect_->readBuffer.read(content, req->getContentLength());
				//LOG_INFO("4." + std::to_string(req->content_.size()) + ":" + std::to_string(spConnect_->readBuffer.size()));
				onMessage();
			}
		}
	}
	return;
}
void HttpHandler::onMessage()
{
	MessagePackagePtr spMessage;
	auto spHttpMessage = std::make_shared<HttpMessagePackage>();
	spHttpMessage->spConnect = spConnect_;
	spHttpMessage->httpRequset.swap(spConnect_->httpRequset_);
	spMessage = spHttpMessage;
	spConnect_->contentLength_ = -1;
	spThread_->getQueue()->push(std::move(spMessage));
}
bool HttpHandler::onWrite(int len)
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


HttpNormalHandler::HttpNormalHandler(ConnectSessionPtr spConnect, ReactorPtr spReactor) :NomalEventHandler(spConnect, spReactor), spHttpConnect_(spConnect)
{

}

void HttpNormalHandler::onRead()
{
	const char * buffer = spConnect_->readBuffer.getReadbuffer();
	//LOG_INFO(buffer);
	const char* index = ::strstr(buffer, "\r\n");
	const char* index1 = ::strstr(buffer, "\r\n\r\n");

	HttpRequest *req = &(spHttpConnect_->httpRequset_);
	if (index1 != nullptr)
	{
		if (spHttpConnect_->contentLength_ < 0)
		{
			HttpParser::parseHttpFirstline(buffer, index - buffer, req);
			HttpParser::parseHttpHeader(buffer + (index - buffer) + 2, index1 - buffer + 4, req);
			spHttpConnect_->contentLength_ = req->getContentLength();

		}
		//LOG_INFO("0." + std::to_string(spConnect_->contentLength_));

		if (req->getContentLength() + (index1 - buffer) <= spConnect_->readBuffer.size())
		{

			std::vector<char> buf(index1 - buffer + 4);
			//LOG_INFO("1." + std::to_string(buf.size()) + ":" + std::to_string(spConnect_->readBuffer.size()));
			spConnect_->readBuffer.read(buf, buf.size());
			//LOG_INFO("2." + std::to_string(buf.size()) + ":" + std::to_string(spConnect_->readBuffer.size()));
			if (req->getContentLength() > 0)
			{
				auto& content = req->getContent();
				content.resize(req->getContentLength() + 1);
				//LOG_INFO("3." + std::to_string(req->content_.size()) + ":" + std::to_string(spConnect_->readBuffer.size()));
				spConnect_->readBuffer.read(content, req->getContentLength());
				//LOG_INFO("4." + std::to_string(req->content_.size()) + ":" + std::to_string(spConnect_->readBuffer.size()));
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
	spThread_->getQueue()->push(std::move(spMessage));
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
