//
// Created by 郝雪 on 2020/3/27.
//

#include "client.h"

void client::receiveHandler() {

    cout << "client recevive handler is starting..." << endl;
    list<string> targetList{};

    unique_lock<mutex> locker{mx, std::defer_lock};

    while (true) {

        if (!targetList.empty()) {

            string event = targetList.front();
            targetList.pop_front();
            stringstream ss{event};
            const int buff_size = 256;
            char buff[buff_size] = {};

            if (event[0] == '1') {
                ss.getline(buff, buff_size, '|');
                ss.getline(buff, buff_size, '|');
                string msg{buff};
                if (msg == LOGIN_SUCCESS) loginState = true;
                cout << msg << endl;

            } else if (event[0] == '2') {
                ss.getline(buff, buff_size, '|');
                ss.getline(buff, buff_size, '|');
                string fromUser{buff};
                ss.getline(buff, buff_size, '|');
                string msg{buff};
                cout << fromUser + ">>" + msg << endl;
            } else {

                cout << "wrong msg:" + event << endl;
            }


        } else {
            locker.lock();//如果mutex已经被其他线程获得 将挂起等待
            cv.wait(locker,
                    [this] { return !eventList.empty(); }); //wait之前 该线程应当获得了mutex 当线程阻塞时，会自动调用unlock 直到收到notify 自动调用lock
            targetList.swap(eventList);
            locker.unlock();
        }


    }
}

int client::setupConnect() {


    //初始化套接字库
    if (startupWSA() == ERR) {
        cout << "startupWSA fail" << endl;
        return ERR;
    }

    //设定server地址
    sockaddr_in serverAddr = {0};
    setSocketAddr(&serverAddr, server_ip.c_str());
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(server_port);
    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (connect(sock, (sockaddr *) &serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        cout << "connect fail" << endl;
        cleanSocket(sock);
        cleanWSA();
        return ERR;
    }

    cout << "connect to " + server_ip + "  " + to_string(server_port) << endl;

    return OK;
}


int client::receiveData(const SOCKET client, char *const buff, const int buffSize) {
    memset(buff, 0, buffSize);
    int len = recv(client, buff, buffSize, 0); //如果在数据copy进buff时出错或网络中断，返回SOCKET_ERR;
    if (len <= 0) {
        cout << "receive data fail" << endl;
        return ERR;
    } else {
        cout << "receive data size is " << len << endl;
        return OK;
    }

}

int client::sendData(const string &data) {

    const char* d = data.data(); //data()与c_str()没有区别
    if(send(sock,d,data.size(),0)==SOCKET_ERROR){
        return ERR;
    }else{
        return OK;
    }

}


void client::push(string &buff) {
    mx.lock();
    eventList.push_back(buff);
    mx.unlock();
    cv.notify_one();
}

void client::selecting() {

    char buff[BUFFER_SIZE] = {};
    cout << "client starts selecting..." << endl;

    while (true) {

        fd_set fdRead;
        FD_ZERO(&fdRead);
        FD_SET(sock, &fdRead);
        timeval timeInterval{1, 0};
        int ret = select(sock + 1, &fdRead, nullptr, nullptr, &timeInterval);
        if (ret < 0) {
            cout << "select err" << endl;
            break;
        }
        if (FD_ISSET(sock, &fdRead)) {
            FD_CLR(sock, &fdRead);
            if (receiveData(sock, buff, sizeof(buff)) == ERR) {
                cleanSocket(sock);
                cleanWSA();
                break;
            }
            string b{buff};
            push(b);
        }

    }

}

int client::login(const string& username,const string& password){


    char buff[BUFFER_SIZE]={};
    string data = "1|"+username+"|"+password;
//    cout<<data<<endl;
    sendData(data);
    if(receiveData(sock, buff, BUFFER_SIZE)==ERR){}
    else{
        const int buff_size = 256;
        char b[buff_size] = {};
        stringstream ss{buff};
        ss.getline(b,buff_size,'|');
        ss.getline(b,buff_size,'|');
        string data{b};

        if(data==LOGIN_SUCCESS){
            loginState = true;
            cout<<LOGIN_SUCCESS<<endl;
            return OK;
        }else {
            cout << LOGIN_ERR << endl;
        }
    }
    return ERR;

}

int client::start() {

    if (setupConnect() == OK) {
        string username = {};
        string password = {};
        const int buff_size = 256;
        char buff[buff_size] = {};
        //这个线程发送信息
        while (true) {

            if (!loginState) {
                cout << "请输入用户名:" << endl;
                memset(buff, 0, buff_size);
                cin.getline(buff, buff_size); //这里能不能用cin<<
                username = buff;
                cout << "请输入密码:" << endl;
                memset(buff, 0, buff_size);
                cin.getline(buff, buff_size);
                password = buff;
                login(username,password);
                //起一个线程selecting
                thread t{&client::selecting, this};
                t.detach();
            }else{

                cout<<"loginState"+to_string(loginState)<<endl;
                memset(buff,0,buff_size);
                cin.getline(buff, buff_size); //直接写成username|msg形式
                string d{buff};
                sendData("2|"+d);
            }


        }
    }
    return ERR;

}