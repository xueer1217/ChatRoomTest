//
// Created by 郝雪 on 2020/3/28.
//

#include "constant.h"
#include "socketUtil.h"

void cleanSocket(SOCKET s) {
#ifdef _WIN32
    closesocket(s);
#else
    close(s);
#endif
}

void cleanWSA() {
#ifdef _WIN32
    WSACleanup();
#else
    // do nothing.
#endif
}

//初始化套接字库
int startupWSA() {
#ifdef _WIN32
    WSADATA wsaData = {};
  if (WSAStartup(SOCKET_VERSION, &wsaData) != OK) {
    return ERR;
  } else {
    return OK;
  }
#else
    return OK;
#endif
}

void setSocketAddr(sockaddr_in* addr, const char* ip) {
    if (ip == nullptr) {
#ifdef _WIN32
        (*addr).sin_addr.S_un.S_addr = htonl(INADDR_ANY);
#else
        (*addr).sin_addr.s_addr = htonl(INADDR_ANY);
#endif
    } else {
#ifdef _WIN32
        (*addr).sin_addr.S_un.S_addr = inet_addr(ip);
#else
        (*addr).sin_addr.s_addr = inet_addr(ip);
#endif
    }
}
