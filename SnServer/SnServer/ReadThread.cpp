#include"stdafx.h"
#include"EchoServer.h"
#include"ReadThread.h"


ReadThread::ReadThread(int listenfd, std::mutex* mutex) :spReactor_(new Reactor),listenFd_(listenfd),mutex_(mutex)
{
}
void ReadThread::init()
{
	spReactor_->wpThisThead_ = shared_from_this();
	timeWheel_.setFunc(std::bind(&ReadThread::onTimerRemoveClient, this, std::placeholders::_1));
	TimeEventPtr spTimeEvent = timeWheel_.getEvent(1000);
	EventHandlerPtr spEventHandler(new AcceptHandler(listenFd_, spReactor_, mutex_));
	spReactor_->AddHandler(spEventHandler);
	EventHandlerPtr spTimeEventHandler(new TimeHandler(spTimeEvent, spReactor_));
	spReactor_->AddHandler(spTimeEventHandler);
}
void ReadThread::loop()
{
	while (true)
	{
		spReactor_->Loop(10);
	}
}
void ReadThread::run()
{
	LOG_INFO("readThread start");
	init();
	loop();
}
void ReadThread::removeClient(int sock)
{
	auto spConnect = connectionManager_[sock];
	spConnect->close();
	timeWheel_.remove(spConnect);
	connectionManager_.erase(sock);
}
void ReadThread::onTimerRemoveClient(ConnectSessionPtr spConnect)
{
	spConnect->close();
	connectionManager_.erase(spConnect->getFd());
}

TimeWheel& ReadThread::getTimeWheel()
{
	return timeWheel_;
}
std::map<int, ConnectSessionPtr>& ReadThread::getManager()
{
	return connectionManager_;
}