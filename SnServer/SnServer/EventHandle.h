#pragma once
#ifndef EVENTHANDLE_H_
#define EVENTHANDLE_H_
#include<memory>

enum EventType
{
	ReadEvent = 0x01,
	WriteEvent = 0x02,
	ErrorEvent = 0x04,
	EventMask = 0xff
};
typedef unsigned int Event;
class EventHandler: public std::enable_shared_from_this<EventHandler>
{
public:
	virtual	~EventHandler() {};
	virtual void ReadHandle() {}
	virtual void WriteHandle() {}
	virtual void ErrorHandle() {}
	virtual void SetHandlerType(Event type) { type_ = type; }
	virtual int GetFd() { return 0; }
	Event GetHandlerType() { return type_; };
protected:
	Event type_;
};
typedef std::shared_ptr<EventHandler> EventHandlerPtr;

class ReadThread;
typedef std::shared_ptr<ReadThread> ReadThreadPtr;
typedef std::weak_ptr<ReadThread> ReadThreadWeakPtr;



#endif
