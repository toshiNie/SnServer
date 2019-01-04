#pragma once
class noncopyable
{
public:
	noncopyable() {};
	~noncopyable() {};
	noncopyable(const noncopyable&) = delete;
	const noncopyable& operator=(const noncopyable&) = delete;
};