#pragma once
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
	virtual int GetFd() {}
	Event GetHandlerType() { return type_; };
protected:
	Event type_;
};
typedef std::shared_ptr<EventHandler> EventHandlerPtr;
typedef std::shared_ptr<std::function<void(EventHandlerPtr)> > FuncHandlePtr;


/*class ListenHandler :public EventHandler
{
	
public:
	ListenHandler(int fd) :
		sock_(fd),
		spReactor_(Reactor::GetReactorPtr())
		{
			EventHandler::SetHandlerType(EventType::ReadEvent);
		}
	void SetFuncHandleListen(FuncHandlePtr spFuncHandleListen)
	{
		spFuncHandleListen_ = spFuncHandleListen;
	}
	void ReadHandle()
	{
		Socket sock = sock_.Accept();
		if (spFuncHandleListen_)
		{
			(*spFuncHandleListen_)(sock);
		}
		spReactor_->AddHandler(shared_from_this());
	}
    void WriteHandle() {};
    void ErrorHandle() {};
private:
	Socket sock_;
	ReactorPtr spReactor_;
	FuncHandleListenPtr spFuncHandleListen_;
};

class AcceptHandler :public EventHandler
{

public:
	AcceptHandler(int fd) :
		sock_(fd),
		spReactor_(Reactor::GetReactorPtr())
	{
		EventHandler::SetHandlerType(EventType::ReadEvent);
	}
	void SetFuncHandleListen(FuncHandlePtr spFuncHandleAccept)
	{
		spFuncHandleAccept_ = spFuncHandleAccept;
	}
	void ReadHandle()
	{
		if (spFuncHandleAccept_)
		{
			(*spFuncHandleAccept_)(shared_from_this());
		}
		if(type_ & WriteEvent)
		{

		}
		
	}
	void WriteHandle() {};
	void ErrorHandle() {};
private:
	Socket sock_;
	ReactorPtr spReactor_;
	FuncHandlePtr spFuncHandleAccept_;

};

class WriteHandler :public EventHandler
{

};
class ReadHandler :public EventHandler
{

};*/