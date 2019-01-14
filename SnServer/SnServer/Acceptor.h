#pragma once
#include"stdafx.h"
#include"TreadRAII.h"
#include"AcceptHandler.h"
class Acceptor
{

private:
	std::vector<ThreadRAII> vecThread_;
};