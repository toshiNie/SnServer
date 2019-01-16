#pragma once
#include"stdafx.h"
class SnTime
{
public:
	static std::time_t getTimeStamp()
	{
		std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds> tp = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());
		auto tmp = std::chrono::duration_cast<std::chrono::milliseconds>(tp.time_since_epoch());
		return tmp.count();
	}

	static std::string millisecondToStr(std::int64_t milliseconds,const char* format)
	{
		std::chrono::milliseconds ms(milliseconds);
		std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds> t1(ms);
		std::time_t t = std::chrono::system_clock::to_time_t(t1);
		auto const msecs = milliseconds % 1000;
		char tmp[100] = { 0 };
		strftime(tmp, sizeof(tmp), format, localtime(&t));
		return tmp;
	}
	static std::string getStrTimeStamp(const char* format = "%Y-%m-%d %H.%M.%S")
	{
		return millisecondToStr(getTimeStamp(),format);

	}
};