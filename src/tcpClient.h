#pragma once

#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <lua.h>
#include <lauxlib.h>
#include <string.h>
#include <stdint.h>
#include <pthread.h>
#include <stdlib.h>

#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

struct socket_buffer {
	void * buffer;
	int sz;
};
bool connectServer(const char* addr, int port);
bool closeServer(int fd);

bool sendMsg(int fd, const char * buffer, int size);//·¢ËÍ
bool recvLoop(int fd);
#endif // !TCPCLIENT_H


