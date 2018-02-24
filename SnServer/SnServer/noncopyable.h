#pragma once
class noncopyable
{
public:
	noncopyable() {};
	~noncopyable() {};
private:
	noncopyable(const noncopyable&) = delete;
	const noncopyable& operator=(const noncopyable&) = delete;
};