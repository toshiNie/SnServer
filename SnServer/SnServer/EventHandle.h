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
	virtual void ReadHandle() = 0;
	virtual void WriteHandle() = 0;
	virtual void ErrorHandle() = 0;
	virtual void SetHandlerType(Event type) { type_ = type; }
	virtual int GetFd() { return 0; }
	Event GetHandlerType() { return type_; };
protected:
	Event type_;
};
typedef std::shared_ptr<EventHandler> EventHandlerPtr;
typedef std::shared_ptr<std::function<void(EventHandlerPtr)> > FuncHandlePtr;


#endif
