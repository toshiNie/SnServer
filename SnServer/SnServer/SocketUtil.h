#pragma once
#ifndef SOCKETUTIL_H_
#define SOCKETUTIL_H_
#include"stdafx.h"
namespace socketutil
{
	int make_socket_non_blocking(int sfd);
	ssize_t writen(int sfd, void *vptr, size_t n);
	size_t readutil(int sfd, void *vptr, const char end);
	ssize_t readn(int sfd, void *vptr, size_t n);
	ssize_t readline(int sfd, void* vptr, size_t maxsize);
}
#endif