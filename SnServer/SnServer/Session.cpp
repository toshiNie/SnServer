#include "stdafx.h"
#include "Session.h"

ConnectSession::ConnectSession(int sock, ReactorPtr spReactor)
	:sock_(sock), spReactor_(spReactor), index_(-1)
{

}
void ConnectSession::setIndex(int index)
{
	index_ = index;
}
int& ConnectSession::getRefIndex()
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