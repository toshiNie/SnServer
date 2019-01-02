#include"stdafx.h"
#include"SocketUtil.h"
#include"AsyncLog.h"
#include"ReadThread.h"
#include"HttpServer.h"
#include"AcceptHandler.h"

void AcceptHandler::readHandle()
{
	LOG_INFO("accept handle");
	//std::lock_guard <std::mutex> lg(*mutex_);
	int sock = ::accept(sock_, nullptr, nullptr);
	LOG_INFO("accept sock: " + std::to_string(sock));
	if (sock < 0)
	{
		return;
	}
	socketutil::make_socket_non_blocking(sock);

	auto spConnect = std::make_shared<HttpConnectSession>(sock, spReactor_);

	//ConnectSessionPtr spConnect(new ConnectSession(sock, spReactor_));
	auto spThisThread = spReactor_->wpThisThead_.lock();
	spThisThread->getTimeWheel().addSock(sock, spConnect->getRefIndex());
	spThisThread->getManager().insert(std::make_pair(sock, spConnect));

	auto spReadHandler = std::make_shared<HttpHandler>(spConnect, spReactor_);

	spReadHandler->setHandlerType(ReadEvent);
	spReactor_->addHandler(spReadHandler);
	LOG_INFO("AddReadHandler: " + std::to_string(sock));
}