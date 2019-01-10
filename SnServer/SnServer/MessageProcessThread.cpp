#include"stdafx.h"
#include"EventHandler.h"
#include"AsyncLog.h"
#include"SnBuffer.h"
#include"Reactor.h"
#include"Session.h"
#include"HttpHandler.h"
#include"HttpResponse.h"
#include"Global.h"
#include"MessageProcessThread.h"


void MessageProcessThread::run()
{
	while (!Global::cancleFlag)
	{
		auto spMessage = spMessageQueue_->pop();
		if (!spMessage)
		{
			continue;
		}
		//LOG_INFO(spMessage->buffer.data() + sizeof(int));
		std::lock_guard<std::mutex> lg(spMessage->spConnect->writeMutex);
		int size = spMessage->size - sizeof(int);
		spMessage->spConnect->writeBuffer.append((const char*)&size, sizeof(int));
		spMessage->spConnect->writeBuffer.append(spMessage->buffer.data() + sizeof(int), spMessage->size - sizeof(int));
		spMessage->spConnect->getReactor()->mod(spMessage->spConnect->getFd(), WriteEvent);

		//HttpMessagePackagePtr spHttpMessage = std::dynamic_pointer_cast<HttpMessagePackage>(spMessage);
		//LOG_INFO() << spHttpMessage->httpRequset.getContent().data();
		//HttpResponse resp;
		//resp.setCode(HttpResponse::C200);
		//std::string ret = "hello world";
		//resp.setContent(ret);
		//std::string strResp = resp.serialize();
		//std::lock_guard<std::mutex> lg(spMessage->spConnect->writeMutex);
		//spMessage->spConnect->writeBuffer.append(strResp.data(), strResp.size());
		//spMessage->spConnect->getReactor()->mod(spMessage->spConnect->getFd(), WriteEvent); 
	}
}