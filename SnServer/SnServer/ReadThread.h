#pragma once
#include"Message.h"
#include"TimeWheel.h"
#include "ThreadLocalManager.h"

class ReadThread : public ThreadLocalManager, public std::enable_shared_from_this<ReadThread>
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

	std::map<int, std::shared_ptr<ConnectSession> > & getManager();

	MessageQueuePtr getQueue();

	virtual void addConnection(std::shared_ptr<ConnectSession> spConnect);
	virtual void removeConnection(std::shared_ptr<ConnectSession> spConnect);
	virtual void resetConnection(std::shared_ptr<ConnectSession> spConnect);
	virtual void pushToQueue(std::shared_ptr<MessagePackage> spMessage);

private:
	struct Impl;
	std::unique_ptr<Impl> upImpl_;

	int listenFd_;
	TimeWheel timeWheel_;
	std::map<int, std::shared_ptr<ConnectSession>> connectionManager_;
	std::mutex* mutex_;
	MessageQueuePtr spQueue_;
};
using ReadThreadPtr = std::shared_ptr<ReadThread>;
using ReadThreadWeakPtr = std::weak_ptr<ReadThread>;