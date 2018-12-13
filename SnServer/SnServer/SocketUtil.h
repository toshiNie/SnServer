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
	ssize_t writen(int sfd, void *vptr, size_t n)
	{
		size_t nleft;
		ssize_t nwrite;
		char *ptr = (char*)vptr;
		nleft = n;
		while (nleft > 0)
		{
			nwrite = write(sfd, ptr, nleft);
			if (nwrite <= 0)
			{
				if (nwrite < 0 && errno == EINTR)
					nwrite = 0;
				else
					return -1;
			}
			nleft -= nwrite;
			ptr += nwrite;
		}
		return n;
	}

	size_t readutil(int sfd, void *vptr, const char end)
	{
		size_t size = 0;
		size_t nread;
		char *ptr = (char*)vptr;
		while (true)
		{
			if (nread = read(sfd, ptr, 1) <= 0)
			{
				std::cout << std::to_string(nread) << std::endl;
				if (nread < 0 && errno == EINTR)
					nread = 0;
				else
					return -1;
			}
			std::cout << ":" + std::to_string(nread) << std::endl;
			if (*ptr == end)
			{
				return size + 1;
			}
			size++;
			ptr += nread;
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

	ssize_t readline(int sfd, void* vptr, size_t maxsize)
	{
		ssize_t  rc;
		char c, *ptr;
		ptr = (char*)vptr;
		int i;
		for (i = 1; i < maxsize; i++)
		{
			if ((rc = read(sfd, &c, 1)) == 1)
			{
				std::cout <<" char: "<< c << std::endl;
				*ptr++ = c;
				if (c == '\n')
					break;
			}
			else if (rc == 0)
			{
				*ptr = 0;
				return (i - 1);
			}
			else
			{
				if (errno == EINTR)
				{
					i--;
					continue;
				}
				return -1;
			}
		}
		*ptr = 0;
		return i;
	}
}