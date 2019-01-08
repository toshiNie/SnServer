#include"stdafx.h"
#include"AsyncLog.h"
#include"ReadThread.h"
#include"NormalHandler.h"


NomalEventHandler::NomalEventHandler(ConnectSessionPtr spConnect, ReactorPtr spReactor) :
	spReactor_(spReactor), spConnect_(spConnect), spThread_(spReactor->wpThisThead_.lock())
{
}
void NomalEventHandler::readHandle()
{
	LOG_DEBUG("Readhanlde");
	spThread_->getTimeWheel().resetSock(spConnect_->getFd(), spConnect_->getRefIndex());
	int remainSize = spConnect_->readBuffer.getRemainSize();
	LOG_DEBUG("remain size: " + std::to_string(remainSize));
	//int r = ::read(spConnect_->getFd(), (void*)buffer.data(), READ_SIZE);
	int r = ::read(spConnect_->getFd(), spConnect_->readBuffer.getRemainbuffer(), remainSize);
	if (r < 0)
	{
		if (errno == EAGAIN)
		{
			//r = 0;
			//spReactor_->mod(spConnect_->getFd(), ReadEvent);
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
		LOG_DEBUG("read  size: " + std::to_string(r));
		spConnect_->readBuffer.peek(r);
		onRead();
		//spReactor_->mod(spConnect_->getFd(), ReadEvent);
	}
	return;
}

void NomalEventHandler::writeHandle()
{
	LOG_DEBUG("TRYLOCK");
	std::lock_guard<std::mutex> lg(spConnect_->writeMutex);
	spThread_->getTimeWheel().resetSock(spConnect_->getFd(), spConnect_->getRefIndex());
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
		spThread_->removeClient(spConnect_->getFd());
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

void NomalEventHandler::errorHandle()
{
	spThread_->removeClient(spConnect_->getFd());
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
	spReactor_->wpThisThead_.lock()->getQueue()->push(std::move(spMessage));
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
