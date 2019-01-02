#pragma once
#ifndef SOCKETUTIL_H_
#define SOCKETUTIL_H_
namespace socketutil
{
	int make_socket_non_blocking(int sfd);
	bool setReuseAddr(int fd);
	bool setReusePort(int fd);
	bool setTcpNoDelay(int fd);
	bool setkeepAlive(int fd);
	ssize_t readv2(int fd, char* buffer1, size_t size1, char* buffer2, size_t size2);
}
#endif