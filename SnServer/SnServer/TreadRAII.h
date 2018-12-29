#pragma once
#include<thread>

class ThreadRAII{
public:
	enum class DtorAction { join, detach }; 
											
	ThreadRAII(std::thread&& t, DtorAction a) 
		: action(a), t(std::move(t)) {} 
	ThreadRAII(ThreadRAII&& thread):
		t(std::move(thread.t)),
		action(std::move(thread.action))
	{

	}
	~ThreadRAII()
	{ 
		if (t.joinable()) { 
			if (action == DtorAction::join) {
				t.join();
			}
			else {
				t.detach();
			}
		}
	}
	std::thread& get() { return t; } 
private:
	DtorAction action;
	std::thread t;
};
using ThreadRAIIPtr = std::unique_ptr<ThreadRAII>;