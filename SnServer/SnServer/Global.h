#pragma once
#include "stdafx.h"
#include<atomic>
class Global
{
public:
	static std::atomic_bool cancleFlag;
};