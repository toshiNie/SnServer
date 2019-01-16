#pragma once
#include"stdafx.h"
#include"SocketUtil.h"
struct Address
{
	std::string ip;
	int port;


	Address(const std::string& strIp, int iPort) :ip(strIp), port(iPort)
	{
	}
	Address(){}
	sockaddr* toAddr(bool isIpv6)
	{
		if (!isIpv6)
		{
			bzero(&addr_.addripv4_, sizeof(sockaddr_in));
			addr_.addripv4_.sin_family = AF_INET;
			addr_.addripv4_.sin_addr.s_addr = inet_addr(ip.c_str());
			addr_.addripv4_.sin_port = htons(port);
			return (struct sockaddr*)&addr_.addripv4_;
		}
		else
		{
			bzero(&addr_.addripv6_, sizeof(sockaddr_in6));
			addr_.addripv6_.sin6_family = AF_INET6;
			inet_pton(AF_INET6, ip.c_str(), &addr_.addripv6_.sin6_addr);
			//addr_.addripv6_.sin6_addr = in6addr_any;
			addr_.addripv6_.sin6_port = htons(port);
			return (struct sockaddr*)&(addr_.addripv6_);
			
		}
	}

	size_t getAddrSize(bool isIpv6)
	{
		return isIpv6 ? sizeof(sockaddr_in6) : sizeof(sockaddr_in);
	}
private: 
	//sockaddr_in addripv4_;
	union addr
	{
		sockaddr_in addripv4_;
		sockaddr_in6 addripv6_;
	} addr_;

};
class Socket 
{
public:
	Socket(bool isIpv6 = false):
		 isIpv6_(isIpv6),
		fd_(::socket(isIpv6? AF_INET6:AF_INET, SOCK_STREAM, 0))
	{
	}
	Socket(int fd) :fd_(fd) {}
	int getSockFd()
	{
		return fd_;
	}
	bool  bindAddress(Address& address)
	{
		return ::bind(fd_, address.toAddr(isIpv6_), address.getAddrSize(isIpv6_)) == 0;
	}
	bool listen(int backlog)
	{
		return ::listen(fd_, backlog) == 0;
	}
	Socket accept(Address& address)
	{
		socklen_t len = address.getAddrSize(isIpv6_);

		int fd=::accept(fd_, address.toAddr(isIpv6_),&len);
		return Socket(fd);
	}
	int accept()
	{
		int fd = ::accept(fd_, nullptr, nullptr);
		return fd;
	}
	void setNonblock()
	{
		socketutil::setNonblocking(fd_);
	}
	bool connect(Address& address)
	{
		socklen_t len = address.getAddrSize(isIpv6_);
		return ::connect(fd_, address.toAddr(isIpv6_), len) >= 0;
	}
	void close()
	{
		::close(fd_);
	}
	~Socket()
	{
		//::close(fd_);
	}
private:
	int fd_;
	bool isIpv6_;
};