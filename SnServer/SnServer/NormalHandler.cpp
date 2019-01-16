#include"stdafx.h"
#include"AsyncLog.h"
#include"LoopThread.h"
#include"NormalHandler.h"


NormalEventHandler::NormalEventHandler(ConnectSessionPtr spConnect, ReactorPtr spReactor) :
	spReactor_(spReactor), spConnect_(spConnect)
{
}
void NormalEventHandler::readHandler()
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
	}
	return;
}

void NormalEventHandler::writeHandler()
{
	auto spConnManager = spReactor_->wpThreadLocalManager.lock();
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

void NormalEventHandler::errorHandler()
{
	 spReactor_->wpThreadLocalManager.lock()->removeConnection(spConnect_);
}


int NormalEventHandler::getFd()
{
	return spConnect_->getFd();
}


void NormalEventHandler::onRead()
{
	int len = *(int*)spConnect_->readBuffer.getReadbuffer();
	if (spConnect_->readBuffer.size() >= sizeof(len) + len)
	{
		onMessage();
	}
}
void NormalEventHandler::onMessage()
{
	int len = *(int*)spConnect_->readBuffer.getReadbuffer();
	auto spMessage = std::make_shared<MessagePackage>();
	spMessage->buffer.resize(len + sizeof(len));
	spConnect_->readBuffer.read(spMessage->buffer, len + sizeof(len));
	spMessage->size = spMessage->buffer.size();
	spMessage->spConnect = spConnect_;
	spReactor_->wpThreadLocalManager.lock()->pushToQueue(spMessage);
}

bool NormalEventHandler::onWrite(int len)
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
