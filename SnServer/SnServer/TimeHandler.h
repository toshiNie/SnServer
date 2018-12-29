#pragma once
#include "EventHandler.h"
#include "TimeEvent.h"

class TimeHandler
	:public EventHandler
{
public:
	TimeHandler(TimeEventPtr spTimeEvent, ReactorPtr spReactor);
	void readHandle() override;
	int getFd() override;
private:
	ReactorPtr spReactor_;
	TimeEventPtr spTimeEvent_;
};