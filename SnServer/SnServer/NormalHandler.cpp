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
	std::vector<char> buffer(READ_SIZE);
	int r = ::read(spConnect_->getFd(), (void*)buffer.data(), READ_SIZE);
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
		LOG_DEBUG("read buffer size: " + std::to_string(spConnect_->readbuffer_.size()));
		if (spConnect_->readbuffer_.size() == 0 && *(int*)buffer.data() == r)
		{
			onMessage(buffer);
			return;
		}

		spConnect_->readbuffer_.append(buffer.data(), r);
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
		std::vector<char> buffer(len + sizeof(len));
		spConnect_->readbuffer_.read(buffer, buffer.size());
		onMessage(buffer);
	}
}
void NomalEventHandler::onMessage(std::vector<char>& buffer)
{
	LOG_DEBUG(buffer.data() + sizeof(int));
	auto spMessage = std::make_shared<MessagePackage>();
	spMessage->buffer.swap(buffer);
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
