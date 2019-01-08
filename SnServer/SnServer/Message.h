#pragma once
#include"Session.h"
#include"SafeQueue.h"
class MessagePackage
{
public:
	int size;
	std::vector<char> buffer;
	std::shared_ptr<ConnectSession> spConnect;
	virtual ~MessagePackage() {}
};
using MessagePackagePtr = std::shared_ptr<MessagePackage>;
using MessageQueue = SafeQueue<MessagePackagePtr>;
using MessageQueuePtr = std::shared_ptr<MessageQueue>;