#pragma once
#include"Session.h"
#include"SafeQueue.h"
class MessagePackage
{
public:
	int size;
	std::vector<char> buffer;
	ConnectSessionPtr spConnect;
	virtual ~MessagePackage() {}

};
using MessagePackagePtr = std::shared_ptr<MessagePackage>;
using MessageQueue = SafeQueue<MessagePackagePtr>;
using MessageQueuePtr = std::shared_ptr<MessageQueue>;