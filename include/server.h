//
// Created by 郝雪 on 2020/3/27.
//

#ifndef CHATROOMTEST_SERVER_H
#define CHATROOMTEST_SERVER_H

#include "socketUtil.h"
#include <map>
#include <string>
#include <sstream>

using std::map;
using std::string;
using std::stringstream;
using std::to_string;


class server {

    int port;
    SOCKET serverSocket{INVALID_SOCKET};
    map<SOCKET, string> socketToUsername{}; //所有建立socket连接的
    map<string, SOCKET> usernameToSocket{};//所有登录用户

    map<string, string> userInfo{
            {"hh", "111"},
            {"xx", "222"}
    };


    int setupConnect();

    void selecting();

    int receiveData(const SOCKET client, char *const buff, const int buffSize);

    void kick(SOCKET client);

    int sendData(const SOCKET &client, const string &data);

    int login(const SOCKET &client, const string &username, const string &password);

public:
    server(int p) : port(p) {};

    int start();
};


#endif //CHATROOMTEST_SERVER_H
