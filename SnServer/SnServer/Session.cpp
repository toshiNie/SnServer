#include "stdafx.h"
#include "SnBuffer.h"
#include "Reactor.h"
#include "Message.h"
#include "ReadThread.h"
#include "Session.h"

ConnectSession::ConnectSession(int sock, ReactorPtr spReactor)
	:sock_(sock), spReactor_(spReactor), index_(-1)
{

}
void ConnectSession::setIndex(int index)
{
	index_ = index;
}
int ConnectSession::getIndex()
{
	return index_;
}
int ConnectSession::getFd()
{
	return sock_;
}

ReactorPtr ConnectSession::getReactor()
{
	return spReactor_;
}
void ConnectSession::close()
{
	::close(sock_);
}

void ConnectSession::onRead()
{
	int len = *(int*)readbuffer_.getReadbuffer();
	//LOG_INFO(std::to_string(len));
	//LOG_INFO(std::to_string(readbuffer_.size()));
	if (readbuffer_.size() >= sizeof(len) + len)
	{
		std::vector<char> buffer(len + sizeof(len));
		readbuffer_.read(buffer, buffer.size());
		//LOG_INFO(buffer.data()+sizeof(int));
		onMessage(buffer);
	}
}

bool ConnectSession::onWrite(int len)
{
	if (len < writebuffer_.size())
	{
		writebuffer_.consumHead(len);
		return false;
	}
	else
	{
		writebuffer_.reset();
		return true;
	}
}
void ConnectSession::onMessage(std::vector<char>& buffer)
{
	MessagePackagePtr spMessage(new MessagePackage());
	spMessage->buffer.swap(buffer);
	spMessage->size = spMessage->buffer.size();
	spMessage->spConnect = shared_from_this();
	spReactor_->wpThisThead_.lock()->getQueue()->push(spMessage);
}
