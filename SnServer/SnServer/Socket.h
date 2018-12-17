#pragma once
#include"stdafx.h"
struct Address
{
	const char* strIp_;
	int port;
	bool isIpv6_;
	sockaddr_in addr_;
	sockaddr* ToIpv4Addr()
	{
		addr_.sin_family = AF_INET;
		addr_.sin_addr.s_addr = inet_addr(strIp_);
		addr_.sin_port = htons(port);
		return (struct sockaddr*)&addr_;
	}
	size_t getAddrSize()
	{
		return sizeof(sockaddr);
	}
};
class Socket
{
public:
	Socket():fd_(::socket(AF_INET, SOCK_STREAM,0)) {}
	Socket(int fd) :fd_(fd) {}
	int GetSockFd()
	{
		return fd_;
	}
	int  BindAddress(Address& address)
	{
		return ::bind(fd_, address.ToIpv4Addr(), address.getAddrSize());
	}
	bool Listen(int listenSize)
	{
		::listen(fd_,listenSize);
		return true;
	}
	Socket Accept(Address& address)
	{
		socklen_t len = address.getAddrSize();

		int fd=::accept(fd_, address.ToIpv4Addr(),&len);
		return Socket(fd);
	}
	int Accept()
	{
		int fd = ::accept(fd_, NULL, NULL);
		return fd;
	}
	~Socket()
	{
		//::close(fd_);
	}
private:
	int fd_;
};