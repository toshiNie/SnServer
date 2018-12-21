#pragma once
#include"TimeWheel.h"
#include"Session.h"
#include"Reactor.h"
#include"MessageProcessThread.h"

class ReadThread : public std::enable_shared_from_this<ReadThread>
{
public:
	ReadThread(int listenfd, MessageQueuePtr spQueue,std::mutex* mutex = NULL);

	void init();

	void loop();

	void run();

	void removeClient(int sock);

	void onTimerRemoveClient(ConnectSessionPtr spConnect);

	TimeWheel& getTimeWheel();

	std::map<int, ConnectSessionPtr>& getManager();

	MessageQueuePtr getQueue()
	{
		return spQueue_;
	}
private:
	int listenFd_;
	ReactorPtr spReactor_;

	TimeWheel timeWheel_;
	std::map<int, ConnectSessionPtr> connectionManager_;
	std::mutex* mutex_;
	MessageQueuePtr spQueue_;

};