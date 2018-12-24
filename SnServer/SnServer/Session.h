#pragma once
#ifndef CONNECTSESSION_H_
#define CONNECTSESSION_H_

class ConnectSession: public std::enable_shared_from_this<ConnectSession>
{
public:
	ConnectSession(int sock, ReactorPtr spReactor);

	void setIndex(int index);

	int getIndex();

	int getFd();

	ReactorPtr getReactor();

	void close();

	void onRead();

	bool onWrite(int len);

	void onMessage(std::vector<char>& buffer);

	SnBuffer readbuffer_;
	SnBuffer writebuffer_;
	std::mutex mutex_;
private:
	int sock_;
	int index_;
	ReactorPtr spReactor_;
	
};
typedef std::shared_ptr<ConnectSession>  ConnectSessionPtr;

#endif // !CONNECTSESSION_H_