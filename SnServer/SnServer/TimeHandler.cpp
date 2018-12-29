#include"stdafx.h"
#include "Reactor.h"
#include"TimeHandler.h"


TimeHandler::TimeHandler(TimeEventPtr spTimeEvent, ReactorPtr spReactor) :
	spReactor_(spReactor),
	spTimeEvent_(spTimeEvent)
{
	setHandlerType(ReadEvent);
}

void TimeHandler::readHandle() 
{
	uint64_t time;
	int r = read(spTimeEvent_->getFd(), (void*)&time, sizeof(time));
	if (r > 0)
	{
		//std::cout << time << std::endl;
	}
	(*spTimeEvent_)();
	if (spTimeEvent_->getCancleFlag())
	{
		spReactor_->remove(spTimeEvent_->getFd());
		::close(spTimeEvent_->getFd());
	}
	else
	{
		spTimeEvent_->setTime(spTimeEvent_->getInterval());
		//spReactor_->addHandler(shared_from_this());
	}
}
int TimeHandler::getFd() 
{
	return spTimeEvent_->getFd();
}