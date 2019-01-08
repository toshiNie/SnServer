#include"stdafx.h"
#include "SocketUtil.h"

namespace socketutil
{
	int setNonblocking(int fd)
	{
		int flags;
		flags = fcntl(fd, F_GETFL, 0);
		if (flags == -1)
		{
			return -1;
		}
		flags |= O_NONBLOCK;
		int ret = fcntl(fd, F_SETFL, flags);
		if (ret == -1)
		{
			return ret;
		}
		return 0;
	}
	bool setReuseAddr(int fd)
	{
		bool bReuseAddr;
		return ::setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &bReuseAddr, sizeof(bReuseAddr)) == 0;
	}

	bool setReusePort(int fd)
	{
		bool bReuseAddr;
		return ::setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, &bReuseAddr, sizeof(bReuseAddr)) == 0;
	}
	bool setTcpNoDelay(int fd)
	{
		int iOption = 1;
		return ::setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &iOption, sizeof(iOption)) == 0;
	}

	bool setkeepAlive(int fd)
	{
		int iOption = 1;
		return ::setsockopt(fd, IPPROTO_TCP, SO_KEEPALIVE, &iOption, sizeof(iOption)) == 0;
	}

	ssize_t readv2(int fd, char* buffer1, size_t size1, char* buffer2, size_t size2)
	{
		struct iovec iov[2];
		iov[0].iov_base = buffer1;
		iov[0].iov_len = size1;
		iov[1].iov_base = buffer2;
		iov[1].iov_len = size2;
		return ::readv(fd, iov, 2);
	}

}