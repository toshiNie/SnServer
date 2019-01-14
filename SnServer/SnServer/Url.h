#pragma once
#include"stdafx.h"
class Url
{
public:
	enum class Protocol
	{
		HTTP,
		HTTPS,
		UNKNOWEN
	};
private:
	Protocol protocol_;
	std::string host_;
	int port_;
	std::vector<std::string> path_;
	std::unordered_map<std::string, std::string> params_;
	bool isIpv6_;
};
