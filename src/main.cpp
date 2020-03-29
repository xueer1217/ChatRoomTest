//
// Created by 郝雪 on 2020/3/28.
//

#include <iostream>
#include <string>
#include <server.h>
#include <client.h>

using std::cout;
using std::cin;
using std::endl;
using std::to_string;
using std::string ;

//不验证输入的正确性



int main(int argc,char* argv[]){


    if(argc<3||argc>4){
        cout<<"please input 【server server_port】or【client server_ip server_port】"<<endl;
        return -1;
    }else if(argc==3){
        string type = argv[1];
        int server_port = atoi(argv[2]);
        cout<<"type:"+type<<";server_port:"+to_string(server_port)<<endl;
        //创建一个server
        server s(server_port);
        s.start();
    } else{
        string type = argv[1];
        string server_ip =argv[2] ;
        int server_port = atoi(argv[3]);
        cout<<"type:"+type<<";server_ip:"+server_ip<<";server_port:"+to_string(server_port)<<endl;
        //创建一个client
        client c(server_ip,server_port);
        c.start();
    }

    return 0;

}