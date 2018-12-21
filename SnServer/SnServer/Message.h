#pragma once
#include"Session.h"
#include"SafeQueue.h"
struct MessagePackage
{
	int size;
	std::vector<char> buffer;
	ConnectSessionPtr spConnect;
};
typedef std::shared_ptr<MessagePackage> MessagePackagePtr;
typedef SafeQueue<MessagePackagePtr> MessageQueue;
typedef std::shared_ptr<SafeQueue<MessagePackagePtr> > MessageQueuePtr;