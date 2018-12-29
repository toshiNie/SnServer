#include"stdafx.h"
#include"LogThread.h"
#include"Session.h"
#include"AcceptHandler.h"
#include"ReadThread.h"

struct ReadThread::Impl
{
	Impl(ReactorPtr spReactor) :spReactor_(spReactor) {}
	ReactorPtr spReactor_;
};


ReadThread::ReadThread(int listenfd, MessageQueuePtr spQueue, std::mutex* mutex)
	:upImpl_(std::make_unique<Impl>(std::make_shared<Reactor>())),
	spQueue_(spQueue),
	listenFd_(listenfd),
	mutex_(mutex)
{
}

ReadThread::ReadThread(int listenfd, std::mutex* mutex)
	:upImpl_(std::make_unique<Impl>(std::make_shared<Reactor>())),
	spQueue_(std::make_shared<MessageQueue>()),
	listenFd_(listenfd),
	mutex_(mutex)
{

}
ReadThread::~ReadThread() = default;
void ReadThread::init()
{
	upImpl_->spReactor_->wpThisThead_ = shared_from_this();

	timeWheel_.setFunc(std::bind(&ReadThread::onTimerRemoveClient, this, std::placeholders::_1));
	TimeEventPtr spTimeEvent = timeWheel_.getEvent(5000);
/*	EventHandlerPtr spTimeEventHandler(new TimeHandler(spTimeEvent, spReactor_));
	spReactor_->addHandler(spTimeEventHandler)*/;
	auto spEventHandler = std::make_shared<AcceptHandler>(listenFd_, upImpl_->spReactor_, mutex_);
	upImpl_->spReactor_->addHandler(spEventHandler);

}
void ReadThread::loop()
{
	while (true)
	{
		upImpl_->spReactor_->loop(10);
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
	upImpl_->spReactor_->remove(sock);
	auto spConnect = connectionManager_[sock];
	spConnect->close();
	timeWheel_.remove(spConnect->getFd(),spConnect->getRefIndex());

	connectionManager_.erase(sock);
}
void ReadThread::onTimerRemoveClient(int sock)
{
	LOG_INFO("kick ass" + std::to_string(sock));
	upImpl_->spReactor_->remove(sock);
	auto spConnect = connectionManager_[sock];
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
MessageQueuePtr ReadThread::getQueue()
{
	return spQueue_;
}