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
	int remainSize = spConnect_->readbuffer_.getRemainSize();
	LOG_DEBUG("remain size: " + std::to_string(remainSize));
	if (remainSize == 0)
	{
		remainSize = spConnect_->readbuffer_.expand();
	}
	LOG_DEBUG("remain size2: " + std::to_string(remainSize));
	//int r = ::read(spConnect_->getFd(), (void*)buffer.data(), READ_SIZE);
	int r = ::read(spConnect_->getFd(), spConnect_->readbuffer_.getRemainbuffer(), remainSize);
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
		spConnect_->readbuffer_.peek(r);
		onRead();
		//spReactor_->mod(spConnect_->getFd(), ReadEvent);
	}
	return;
}

void NomalEventHandler::writeHandle()
{
	LOG_DEBUG("TRYLOCK");
	std::lock_guard<std::mutex> lg(spConnect_->mutex_);
	spThread_->getTimeWheel().resetSock(spConnect_->getFd(), spConnect_->getRefIndex());
	LOG_DEBUG("GETLOCK");
	int w = ::write(spConnect_->getFd(), spConnect_->writebuffer_.getReadbuffer(), spConnect_->writebuffer_.size());
	LOG_INFO("writebuffer size: " + std::to_string(spConnect_->writebuffer_.size()));
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
	int len = *(int*)spConnect_->readbuffer_.getReadbuffer();
	if (spConnect_->readbuffer_.size() >= sizeof(len) + len)
	{
		onMessage();
	}
}
void NomalEventHandler::onMessage()
{
	int len = *(int*)spConnect_->readbuffer_.getReadbuffer();
	auto spMessage = std::make_shared<MessagePackage>();
	spMessage->buffer.resize(len + sizeof(len));
	spConnect_->readbuffer_.read(spMessage->buffer, len + sizeof(len));
	spMessage->size = spMessage->buffer.size();
	spMessage->spConnect = spConnect_;
	spReactor_->wpThisThead_.lock()->getQueue()->push(std::move(spMessage));
}

bool NomalEventHandler::onWrite(int len)
{
	if (len < spConnect_->writebuffer_.size())
	{
		spConnect_->writebuffer_.consumHead(len);
		return false;
	}
	else
	{
		spConnect_->writebuffer_.reset();
		return true;
	}
}
