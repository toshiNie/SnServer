#pragma once
#include"stdafx.h"
#include"LogThread.h"
#include"SnBuffer.h"
#include"Session.h"
#include"Message.h"
#include"Global.h"



class MessageProcessThread
{
public:
	MessageProcessThread(MessageQueuePtr spQueue) :spMessageQueue_(spQueue) {}

	void run()
	{
		while (auto spMessage = spMessageQueue_->pop())
		{
			LOG_INFO(spMessage->buffer.data() + sizeof(int));
			spMessage->spConnect->writebuffer_.append(spMessage->buffer.data() + sizeof(int), spMessage->size-sizeof(int));
			spMessage->spConnect->getReactor()->Mod(spMessage->spConnect->getFd(), WriteEvent);
		}
	}
private:
	MessageQueuePtr spMessageQueue_;
};
typedef std::shared_ptr<MessageProcessThread> WorkerPtr;