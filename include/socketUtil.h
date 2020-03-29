//
// Created by 郝雪 on 2020/3/28.
//

#ifndef CHATROOMTEST_SOCKETUTIL_H
#define CHATROOMTEST_SOCKETUTIL_H

#ifdef _WIN32
#include <winsock2.h>
#include <WS2tcpip.h>
#else
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
#include <cstdint>


#define SOCKET int
#define INVALID_SOCKET (SOCKET)(~0) //将0转化成socket类型 再将0所占空间全部转化成1
#define SOCKET_ERROR		   (-1)
#endif


void cleanWSA();
void cleanSocket(SOCKET s);
int startupWSA();
void setSocketAddr(sockaddr_in* addr, const char* ip);


#endif //CHATROOMTEST_SOCKETUTIL_H
