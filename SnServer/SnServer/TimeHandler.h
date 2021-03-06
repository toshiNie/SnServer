#pragma once
#include "EventHandler.h"
#include "TimeEvent.h"

class TimeHandler
	:public EventHandler
{
public:
	TimeHandler(TimeEventPtr spTimeEvent, ReactorPtr spReactor);
	void readHandler() override;
	int getFd() override;
private:
	ReactorPtr spReactor_;
	TimeEventPtr spTimeEvent_;
};