#pragma once
#include"Message.h"
#include"TimeWheel.h"
#include "ThreadLocalManager.h"

class LoopThread : public ThreadLocalManager, public std::enable_shared_from_this<LoopThread>
{
public:
	LoopThread(int listenfd, MessageQueuePtr spQueue,std::mutex* mutex = nullptr);

	LoopThread(int listenfd, std::mutex* mutex = nullptr);

	~LoopThread();

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