#pragma once
#include"stdafx.h"
#include"Message.h"

class MessageProcessThread
{
public:
	MessageProcessThread(MessageQueuePtr spQueue) :spMessageQueue_(spQueue) {}

	void run();


private:
	//void sendback(std::shared_ptr<ConnectSession> spConnection, SnBuffer&& buffer);

	MessageQueuePtr spMessageQueue_;
};
typedef std::shared_ptr<MessageProcessThread> WorkerPtr;