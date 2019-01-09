#pragma once
#ifndef EVENTHANDLE_H_
#define EVENTHANDLE_H_
#include<memory>

enum EventType
{
	ReadEvent = 0x01,
	WriteEvent = 0x02
};
typedef unsigned int Event;
class EventHandler: public std::enable_shared_from_this<EventHandler>
{
public:
	virtual	~EventHandler() {};
	virtual void readHandler() {}
	virtual void writeHandler() {}
	virtual void errorHandler() {}
	virtual void setHandlerType(Event type) { type_ = type; }
	virtual int getFd() = 0;
	Event getHandlerType() { return type_; };
protected:
	Event type_;
};
using EventHandlerPtr = std::shared_ptr<EventHandler>;


#endif
