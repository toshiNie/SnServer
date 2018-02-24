#pragma once
#include"stdafx.h"
namespace socketutil
{
	int make_socket_non_blocking(int sfd)
	{
		int flags;
		flags = fcntl(sfd, F_GETFL, 0);
		if (flags == -1)
		{
			return -1;
		}
		flags |= O_NONBLOCK;
		int ret = fcntl(sfd, F_SETFL, flags);
		if (ret == -1)
		{
			return ret;
		}
		return 0;
	}
	ssize_t readn(int sfd, void *vptr, size_t n)
	{
		size_t nleft;
		ssize_t nread;
		char *ptr = (char*)vptr;
		nleft = n;
		while (nleft > 0)
		{
			if (nread = read(sfd, ptr, nleft) < 0)
			{
				if (errno == EINTR)
					nread = 0;
				else
					return -1;
			}
			else if (nread == 0)
			{
				break;
			}
			nleft -= nread;
			ptr += nread;
		}
		return n - nleft;
	}
	ssize_t writen(int sfd, void *vptr, size_t n)
	{
		size_t nleft;
		ssize_t nwrite;
		char *ptr = (char*)vptr;
		nleft = n;
		while (nleft > 0)
		{
			if (nwrite = write(sfd, ptr, nleft) <= 0)
			{
				if (nwrite < 0 && errno == EINTR)
					nwrite = 0;
				else
					return -1;
			}
			nleft -= nwrite;
			ptr += nread;
		}
		return n;
	}
}