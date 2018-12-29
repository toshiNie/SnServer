#pragma once

#include <atomic>
#include"TimeEvent.h"

class TimeWheel
{
public:
	TimeWheel(int size = 3);

	void setFunc(std::function<void(int)> && func);

	void onTimer();

	TimeEventPtr getEvent(int interval);

	int addSock(int sock, int& refIndex);

	int resetSock(int sock, int& refIndex);

	void remove(int sock, int& refIndex);

private:
	std::vector<std::set<int>> bucket_;
	std::atomic_int index_;
	std::function<void(int)> func_;
};