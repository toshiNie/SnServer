#include"stdafx.h"
#include"EventHandle.h"
#include"LogThread.h"
#include"SnBuffer.h"
#include"Reactor.h"
#include"Session.h"
#include"MessageProcessThread.h"


void MessageProcessThread::run()
{
	while (auto spMessage = spMessageQueue_->pop())
	{
		LOG_INFO(spMessage->buffer.data() + sizeof(int));
		std::lock_guard<std::mutex> lg(spMessage->spConnect->mutex_);
		spMessage->spConnect->writebuffer_.append(spMessage->buffer.data() + sizeof(int), spMessage->size - sizeof(int));
		spMessage->spConnect->getReactor()->Mod(spMessage->spConnect->getFd(), WriteEvent);
	}
}