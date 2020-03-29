//
// Created by 郝雪 on 2020/3/27.
//
//
#include "server.h"
#include <iostream>
#include "constant.h"
#include "socketUtil.h"

using std::cout;
using std::cin;
using std::endl;




int server::setupConnect() {

    //初始化套接字库
    if (startupWSA() == ERR) {
        cout << "start up WSA fail" << endl;
        return ERR;
    }

    //设定socketAddr
    //列表初始化初始化 数组、类、结构
    sockaddr_in sockAddr = {0};
    setSocketAddr(&sockAddr, nullptr);
    sockAddr.sin_family = AF_INET;
    sockAddr.sin_port = htons(port);

    serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    //bind
    if (bind(serverSocket, (sockaddr *) &sockAddr, sizeof(sockaddr_in)) == SOCKET_ERROR) {
        cout << "bind fail" << endl;
        cleanSocket(serverSocket);
        cleanWSA();
        return ERR;
    }

    //将套接字设定为listen模式等待连接请求

    if (listen(serverSocket, BACKLOG) == SOCKET_ERROR) {
        cout << "listen fail" << endl;
        cleanSocket(serverSocket);
        cleanWSA();
        return ERR;
    }

    cout << "setup connection success" << "server is listening [" << port << "]...." << endl;

    return OK;
}

int server::receiveData(const SOCKET client, char *const buff, const int buffSize) {
    memset(buff, 0, buffSize);
    int len = recv(client, buff, buffSize, 0); //如果在数据copy进buff时出错或网络中断，返回SOCKET_ERR;
    if (len <= 0) {
        cout << "receive data fail" << endl;
        return ERR;
    } else {
        cout << "receive data size is " << len << endl;
        cout<< buff <<endl;
        return OK;
    }

}

int server::login(const SOCKET &client, const string &username, const string &password) {
    auto it = userInfo.find(username);
    if (it == userInfo.end()) {
        cout << "username:" + username + "not exist" << endl;
        return ERR;
    } else if (it->second == password) {

        socketToUsername[client] = username;
        usernameToSocket[username] = client;
        cout << "user :" + username + "login success" << endl;
        return OK;
    } else {
        cout << "password is wrong" << endl;
        return ERR;
    }

}
int server::sendData(const SOCKET &client, const string &data){
    const char* d = data.data();
    if(send(client,d,data.size(),0)==SOCKET_ERROR){
        cout<<"send msg fail,socket is"+to_string(client)<<endl;
        return ERR;
    }else{
        cout<<"send msg:"+data+" for socket:"+to_string(client)<<endl;
        return OK;
    }
}

void server::kick(SOCKET client) {

    auto c = socketToUsername.find(client);
    if (c != socketToUsername.end()) {

        string username = c->second;
        if (!username.empty())
            usernameToSocket.erase(username);
        socketToUsername.erase(client);
        cleanSocket(client);
    }
}

void server::selecting() {
    cout << "server is selecting..." << endl;

    char buff[BUFFER_SIZE] = {0};
    SOCKET maxSocket = 0;


    while (true) {

        fd_set fdRead;
        fd_set fdWrite;
        fd_set fdExcept;

        FD_ZERO(&fdRead);
        FD_ZERO(&fdWrite);
        FD_ZERO(&fdExcept);

        FD_SET(serverSocket, &fdRead);
        FD_SET(serverSocket, &fdWrite);
        FD_SET(serverSocket, &fdExcept);

        maxSocket = serverSocket;

        for (auto each:socketToUsername) {
            SOCKET it = each.first;
            FD_SET(it, &fdRead);
            if (it > maxSocket) maxSocket = it;
        }
        timeval timeInterval = {1, 0};
        int ret = select(maxSocket + 1, &fdRead, &fdWrite, &fdExcept,
                         &timeInterval);//检查每个fd，返回值：只有符合条件的fd仍为1；否则为0;如果失败返回SOCKET_ERR

        if (ret < 0) {
            cout << "select fail" << endl;
            break;
        }

        if (FD_ISSET(serverSocket, &fdRead)) {

            FD_CLR(serverSocket,&fdRead);
            cout << "waiting for connection" << endl;

            sockaddr_in socketAddr = {0};
            socklen_t len = sizeof(sockaddr_in);
            SOCKET clientSocket{INVALID_SOCKET};
            clientSocket = accept(serverSocket, (sockaddr *) &socketAddr, &len); //如果失败，返回INVAILD_SOCKET

            if (INVALID_SOCKET == clientSocket) {
                cout << "accept fail" << endl;
            } else {
                socketToUsername[clientSocket] = "";
                cout << "accept success,socket is " << clientSocket << endl;
            }

        }

        for (auto each:socketToUsername) {
            SOCKET it = each.first;
            string itUsername = each.second;
            if (FD_ISSET(it, &fdRead)) {
                if (receiveData(it, buff, BUFFER_SIZE) == ERR) { //如果fd为1，recv为<=0,则判断socket断开
                    kick(it);
                } else {
                    string data{buff};
                    const int buff_size = 256;
                    char b[buff_size] = {};
                    stringstream ss{data};

                    //Login req
                    if (data[0] == '1') {

                            //login
                            ss.getline(b, buff_size, '|');
                            ss.getline(b, buff_size, '|');
                            string username{b};
                            ss.getline(b, buff_size, '|');
                            string password{b};
                            if(login(it,username,password)==OK){
                                sendData(it,"1|" LOGIN_SUCCESS);
                            }else{
                                sendData(it,"1|" LOGIN_ERR);
                            }
                            //send msg
                    }else if(data[0] == '2'){

                        ss.getline(b, buff_size, '|');
                        ss.getline(b, buff_size, '|');
                        string toUsername{b};
                        ss.getline(b, buff_size, '|');
                        string msg{b};

                        if(itUsername.empty()){
                            sendData(it,"2|SERVER|" NO_LOGIN);
                        }else{
                            auto to = usernameToSocket.find(toUsername);
                            if(to != usernameToSocket.end()){
                                sendData(to->second,"2|"+itUsername+"|"+msg);
                            }else{
                                sendData(it,"2|SERVER|" NO_USER);
                            }
                        }

                    }else{
                        cout << "receive wrong msg" << endl;
                    }

                }

            }
        }

        maxSocket = 0;


    }


}


int server::start() {

    cout << "server is starting" << endl;

    if (setupConnect() == OK) {
        selecting();
    } else
        return ERR;

    return OK;
}


