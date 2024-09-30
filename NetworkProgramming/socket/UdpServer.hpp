#pragma once
#include <cstring>

#include <iostream>
#include <string>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

using namespace std;

static const string DefaultIp = "0.0.0.0"; // 默认ip地址
static const int buffersize = 1024;        // 缓冲区的大小

// 枚举进程退出码
enum
{
    SOCKET_ERR = 1,
    BIND_ERR,
};

// 编写udp服务端
class UdpServer
{
public:
    UdpServer(const uint16_t &port, const string &ip = DefaultIp)
        : _ip(ip), _port(port)
    {
    }

    // 服务端初始化
    void init()
    {
        // 1.创建套接字
        _sockfd = socket(AF_INET, SOCK_DGRAM, 0);
        if (_sockfd == -1)
        {
            cerr << "socket error:" << errno << ":" << strerror(errno) << endl;
            exit(SOCKET_ERR);
        }
        cout << "socket success:" << _sockfd << endl;
        // 2.bing套接字
        struct sockaddr_in local;
        bzero(&local, sizeof(local));                   // 对local变量全部置0
        local.sin_addr.s_addr = inet_addr(_ip.c_str()); // 指定其他client只能往这个ip发送信息
        local.sin_addr.s_addr = htonl(INADDR_ANY);      // 指定其他client能往这个服务端上的任何ip发送信息，服务器的真实写法
        local.sin_family = AF_INET;                     // 说明是网络通信
        local.sin_port = htons(_port);                  // 输入端口号

        int ret = bind(_sockfd, (struct sockaddr *)&local, sizeof(local)); // 绑定套接字
        if (ret == -1)
        {
            cerr << "socket bind:" << errno << ":" << strerror(errno) << endl;
            exit(BIND_ERR);
        }
    }

    // 服务端启动
    void start()
    {
        char buffer[buffersize];
        for (;;)
        {
            struct sockaddr_in peer;
            socklen_t len = sizeof(peer);
            ssize_t s = recvfrom(_sockfd, buffer, sizeof(buffer) - 1, 0, (struct sockaddr *)&peer, &len);
            if (s > 0)
            {
                buffer[s] = 0;
                string clientip = inet_ntoa(peer.sin_addr);
                uint16_t clientport = ntohs(peer.sin_port);
                string message = buffer;
                cout << clientip << "[" << clientport << "]" << message << endl;
            }
        }
    }

    ~UdpServer()
    {
    }

private:
    int _sockfd;    // 创建一个套接字后，会返回一个文件描述符
    string _ip;     // 服务端的ip地址
    uint16_t _port; // 服务端的端口号
};