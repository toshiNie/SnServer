#include"stdafx.h"
#include"AsyncLog.h"
#include"ReadThread.h"
#include"NormalHandler.h"


NomalEventHandler::NomalEventHandler(ConnectSessionPtr spConnect, ReactorPtr spReactor) :
	spReactor_(spReactor), spConnect_(spConnect)
{
}
void NomalEventHandler::readHandler()
{
	LOG_DEBUG() <<"Readhanlde";
	auto spConnManager = spReactor_->wpThreadLocalManager.lock();
	spConnManager ->resetConnection(spConnect_);
	int remainSize = spConnect_->readBuffer.getRemainSize();
	LOG_DEBUG() << "remain size: " << remainSize;
	int r = ::read(spConnect_->getFd(), spConnect_->readBuffer.getRemainbuffer(), remainSize);
	if (r < 0)
	{
		if (errno == EAGAIN)
		{
			//r = 0;
			//spReactor_->mod(spConnect_->getFd(), ReadEvent);
			return;
		}
		LOG_ERROR() << "read error: " << errno;
		spConnManager->removeConnection(spConnect_);
	}
	else if (r == 0)
	{
		LOG_INFO() << "client disconnect: " << spConnect_->getFd();
		spConnManager->removeConnection(spConnect_);
	}
	else
	{
		LOG_DEBUG() << "read  size: " << r;
		spConnect_->readBuffer.peek(r);
		onRead();
		//spReactor_->mod(spConnect_->getFd(), ReadEvent);
	}
	return;
}

void NomalEventHandler::writeHandler()
{
	auto spConnManager = spReactor_->wpThreadLocalManager.lock();
	std::lock_guard<std::mutex> lg(spConnect_->writeMutex);
	spConnManager->resetConnection(spConnect_);
	int w = ::write(spConnect_->getFd(), spConnect_->writeBuffer.getReadbuffer(), spConnect_->writeBuffer.size());
	LOG_INFO() << "writebuffer size: " << spConnect_->writeBuffer.size();
	LOG_INFO() << "writen size: " << w;
	if (w < 0)
	{
		if (errno == EAGAIN)
		{
			return;
		}
		LOG_ERROR() << "write error: " << errno;
		spConnManager->removeConnection(spConnect_);
	}
	else
	{
		if (onWrite(w))
		{
			spReactor_->mod(spConnect_->getFd(), ReadEvent);
		}
	}

}

void NomalEventHandler::errorHandler()
{
	 spReactor_->wpThreadLocalManager.lock()->removeConnection(spConnect_);
}


int NomalEventHandler::getFd()
{
	return spConnect_->getFd();
}


void NomalEventHandler::onRead()
{
	int len = *(int*)spConnect_->readBuffer.getReadbuffer();
	if (spConnect_->readBuffer.size() >= sizeof(len) + len)
	{
		onMessage();
	}
}
void NomalEventHandler::onMessage()
{
	int len = *(int*)spConnect_->readBuffer.getReadbuffer();
	auto spMessage = std::make_shared<MessagePackage>();
	spMessage->buffer.resize(len + sizeof(len));
	spConnect_->readBuffer.read(spMessage->buffer, len + sizeof(len));
	spMessage->size = spMessage->buffer.size();
	spMessage->spConnect = spConnect_;
	spReactor_->wpThreadLocalManager.lock()->pushToQueue(spMessage);
}

bool NomalEventHandler::onWrite(int len)
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
