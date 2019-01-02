#include"stdafx.h"
#include"EventHandler.h"
#include"AsyncLog.h"
#include"SnBuffer.h"
#include"Reactor.h"
#include"Session.h"
#include"HttpServer.h"
#include"HttpResponse.h"
#include"MessageProcessThread.h"


void MessageProcessThread::run()
{
	while (auto spMessage = spMessageQueue_->pop())
	{
		/*LOG_INFO(spMessage->buffer.data() + sizeof(int));
		std::lock_guard<std::mutex> lg(spMessage->spConnect->mutex_);
		spMessage->spConnect->writebuffer_.append(spMessage->buffer.data() + sizeof(int), spMessage->size - sizeof(int));
		spMessage->spConnect->getReactor()->mod(spMessage->spConnect->getFd(), WriteEvent);*/

		HttpMessagePackagePtr spHttpMessage = std::dynamic_pointer_cast<HttpMessagePackage>(spMessage);
		LOG_INFO(spHttpMessage->httpRequset.getContent().data());

		HttpResponse resp;
		resp.setCode(HttpResponse::C200);
		resp.setContent("hello world");
		std::string strResp = resp.serialize();
		std::lock_guard<std::mutex> lg(spMessage->spConnect->mutex_);
		spMessage->spConnect->writebuffer_.append(strResp.data(), strResp.size());
		spMessage->spConnect->getReactor()->mod(spMessage->spConnect->getFd(), WriteEvent); 
	}
}