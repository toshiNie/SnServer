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
#include <fstream>  


void sendback(std::shared_ptr<ConnectSession> spConnection, std::shared_ptr<SnBuffer> spBuffer)
{
	spConnection->writeBuffer.append(std::move(*spBuffer));
	spConnection->getReactor()->mod(spConnection->getFd(), WriteEvent);
}

void MessageProcessThread::run()
{
	while (!Global::cancleFlag)
	{
		auto spMessage = spMessageQueue_->pop();
		//LOG_INFO(spMessage->buffer.data() + sizeof(int));
		//SnBuffer buffer;
		//int size = spMessage->size - sizeof(int);
		//buffer.append((const char*)&size, sizeof(int));
		//buffer.append(spMessage->buffer.data() + sizeof(int), spMessage->size - sizeof(int));
		//spMessage->spConnect->getReactor()->addNoticeEvent(std::bind(&MessageProcessThread::sendback, this, std::move(spMessage->spConnect), std::move(buffer)));


		HttpMessagePackagePtr spHttpMessage = std::dynamic_pointer_cast<HttpMessagePackage>(spMessage);
		LOG_INFO() << spHttpMessage->httpRequset.getContent().data();
		HttpResponse resp;
		resp.setCode(HttpResponse::C200);
		std::ifstream fileStream("GitHub.html", std::ios::in);
		std::stringstream ss;
		ss << fileStream.rdbuf();
		fileStream.close();
		resp.setContent(ss.str());
		std::string strResp = resp.serialize();
		std::shared_ptr<SnBuffer> spBuffer(new SnBuffer);
		spBuffer->append(strResp.data(), strResp.size());
		spMessage->spConnect->getReactor()->addNoticeEvent(std::bind(&sendback, spMessage->spConnect, spBuffer));
	}
	
}
