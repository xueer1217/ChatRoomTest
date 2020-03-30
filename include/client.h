//
// Created by 郝雪 on 2020/3/27.
//

#ifndef CHATROOMTEST_CLIENT_H
#define CHATROOMTEST_CLIENT_H

#include <string>
#include "socketUtil.h"
#include <list>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <iostream>
#include <sstream>
#include "constant.h"


using std::list;
using std::string;
using std::thread;
using std::condition_variable;
using std::mutex;
using std::cout;
using std::cin;
using std::endl;
using std::unique_lock;
using std::stringstream;
using std::to_string;

class client {

    string server_ip;
    int server_port;
    SOCKET sock = {INVALID_SOCKET};

    bool loginState = false;

    list<string> eventList{};
    condition_variable cv{};
    mutex mx{};


    int login(const string &username, const string &password);

    void receiveHandler();

    int setupConnect();

    void selecting();

    int receiveData(const SOCKET client, char *const buff, const int buffSize);

    int sendData(const string &data);

    void push(string &buff);

public:
    client(const string &ip, const int &port) : server_ip(ip), server_port(port) {
        thread t(&client::receiveHandler, this);
        t.detach();
    };

    int start();

};

#endif //CHATROOMTEST_CLIENT_H
