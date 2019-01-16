#include"stdafx.h"
#include"AsyncLog.h"
#include"Session.h"
#include"AcceptHandler.h"
#include"Global.h"
#include"HttpHandler.h"
#include"TimeHandler.h"
#include"Reactor.h"
#include"LoopThread.h"

struct LoopThread::Impl
{
	Impl(ReactorPtr spReactor) :spReactor_(spReactor) {}
	ReactorPtr spReactor_;
};


LoopThread::LoopThread(int listenfd, MessageQueuePtr spQueue, std::mutex* mutex)
	:upImpl_(std::unique_ptr<Impl>(new Impl(std::make_shared<Reactor>()))),
	spQueue_(spQueue),
	listenFd_(listenfd),
	mutex_(mutex)
{
}

LoopThread::LoopThread(int listenfd, std::mutex* mutex)
	:upImpl_(std::unique_ptr<Impl>(new Impl(std::make_shared<Reactor>()))),
	spQueue_(std::make_shared<MessageQueue>()),
	listenFd_(listenfd),
	mutex_(mutex)
{

}

LoopThread::~LoopThread() = default;

void LoopThread::init()
{
	upImpl_->spReactor_->wpThreadLocalManager = shared_from_this();
	timeWheel_.setFunc(std::bind(&LoopThread::onTimerRemoveClient, this, std::placeholders::_1));
	TimeEventPtr spTimeEvent = timeWheel_.getEvent(5000);
	EventHandlerPtr spTimeEventHandler(new TimeHandler(spTimeEvent, upImpl_->spReactor_));
	//upImpl_->spReactor_->addHandler(spTimeEventHandler);
	auto spEventHandler = std::make_shared<AcceptHandler<HttpNormalHandler> >(listenFd_, upImpl_->spReactor_);
	upImpl_->spReactor_->addHandler(spEventHandler);

}
void LoopThread::loop()
{
	while (!Global::cancleFlag)
	{
		upImpl_->spReactor_->loop(10);
	}
}
void LoopThread::run()
{
	LOG_INFO() << "readThread start";
	init();
	loop();
}
void LoopThread::removeClient(int sock)
{
	upImpl_->spReactor_->remove(sock);
	auto iter = connectionManager_.find(sock);
	if (iter != connectionManager_.end())
	{
		iter->second->close();
		timeWheel_.remove(iter->second->getFd(), iter->second->getRefIndex());
		connectionManager_.erase(sock);
	}
}
void LoopThread::onTimerRemoveClient(int sock)
{
	LOG_INFO() << "kick ass" << sock;
	upImpl_->spReactor_->remove(sock);
	auto iter = connectionManager_.find(sock);
	if (iter != connectionManager_.end())
	{
		iter->second->close();
		connectionManager_.erase(sock);
	}
}


void LoopThread::addConnection(std::shared_ptr<ConnectSession> spConnect)
{
	connectionManager_[spConnect->getFd()] = spConnect;
	timeWheel_.addSock(spConnect->getFd(), spConnect->getRefIndex());
}
void LoopThread::removeConnection(std::shared_ptr<ConnectSession> spConnect)
{
	int sock = spConnect->getFd();
	upImpl_->spReactor_->remove(sock);
	auto iter = connectionManager_.find(sock);
	if (iter != connectionManager_.end())
	{
		iter->second->close();
		timeWheel_.remove(iter->second->getFd(), iter->second->getRefIndex());
		connectionManager_.erase(sock);
	}
}
void LoopThread::resetConnection(std::shared_ptr<ConnectSession> spConnect)
{
	timeWheel_.resetSock(spConnect->getFd(), spConnect->getRefIndex());
}

void LoopThread::pushToQueue(std::shared_ptr<MessagePackage> spMessage)
{
	spQueue_->push(std::move(spMessage));
}

TimeWheel& LoopThread::getTimeWheel()
{
	return timeWheel_;
}
std::map<int, std::shared_ptr<ConnectSession> >& LoopThread::getManager()
{
	return connectionManager_;
}
MessageQueuePtr LoopThread::getQueue()
{
	return spQueue_;
}