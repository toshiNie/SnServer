#pragma once
#include<memory>
class ConnectSession;
class MessagePackage;
class ThreadLocalManager: public std::enable_shared_from_this<ThreadLocalManager>
{
public:
	ThreadLocalManager() {};
	virtual void addConnection(std::shared_ptr<ConnectSession> spConnect) = 0;
	virtual void removeConnection(std::shared_ptr<ConnectSession> spConnect) = 0;
	virtual void resetConnection(std::shared_ptr<ConnectSession> spConnect) {};
	virtual void pushToQueue(std::shared_ptr<MessagePackage> spMessage) {};
};