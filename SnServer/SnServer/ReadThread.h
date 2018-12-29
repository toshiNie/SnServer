#pragma once
#include"Message.h"
#include"TimeWheel.h"

class ReadThread : public std::enable_shared_from_this<ReadThread>
{
public:
	ReadThread(int listenfd, MessageQueuePtr spQueue,std::mutex* mutex = nullptr);

	ReadThread(int listenfd, std::mutex* mutex = nullptr);

	~ReadThread();

	void init();

	void loop();

	void run();

	void removeClient(int sock);

	void onTimerRemoveClient(int sock);

	TimeWheel& getTimeWheel();

	std::map<int, ConnectSessionPtr>& getManager();

	MessageQueuePtr getQueue();

private:
	struct Impl;
	std::unique_ptr<Impl> upImpl_;

	int listenFd_;
	TimeWheel timeWheel_;
	std::map<int, ConnectSessionPtr> connectionManager_;
	std::mutex* mutex_;
	MessageQueuePtr spQueue_;
};
typedef std::shared_ptr<ReadThread> ReadThreadPtr;
typedef std::weak_ptr<ReadThread> ReadThreadWeakPtr;