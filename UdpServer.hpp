//保证这个头文件只会被包含（编译）一次，防止重复包含导致编译错误
#pragma once

#include <iostream>
#include <string>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "log.hpp"
#include <cstdint>  // 定义了 uint16_t、int32_t 等类型


using namespace LogModule;

const int defaultfd = -1;

class UdpServer
{
public:
    UdpServer(std::string &ip, uint16_t &port)
    :_sockfd(defaultfd),
    _ip(ip),
    _port(port),
    _isrunning(false)
    {}

    void init()
    {
        // 1. 创建套接字
        _sockfd = socket(AF_INET, SOCK_DGRAM, 0);
        if(_sockfd < 0)
        {
            LOG(LogLevel::FATAL) << "socket error~";
            exit(1);
        }
        LOG(LogLevel::INFO) << "socket success, sockfd : " << _sockfd;
        
        // 2. 绑定socket信息，ip和端口，ip(比较特殊)
        // 2.1 填充sockaddr_in结构体
        struct sockaddr_in local;
        // 给local结构体初始化
        bzero(&local, sizeof(local));
        local.sin_family = AF_INET;
        // IP信息和端口信息，要发送到网络；
        // 所以要将本地格式->网络格式；
        local.sin_port = htons(_port);
        // IP转成四字节；
        // 四字节转成网络序列；
        // 用in_addr_t inet_addr(const char *cp)来实现以上两个功能；
        local.sin_addr.s_addr = inet_addr(_ip.c_str());
        // inet_pton()替代，功能更强、错误处理更完善;
        // inet_pton(AF_INET, _ip.c_str(), &local.sin_addr);

        int n = bind(_sockfd, (struct sockaddr*)&local, sizeof(local));
        if(n < 0)
        {
            LOG(LogLevel::FATAL) << "bind error";
            exit(2);
        }
        LOG(LogLevel::INFO) << "bind success, sockfd : " << _sockfd;
    }
    void start()
    {
        // 启动服务器而服务器必须一直工作；
        // 所以是死循环；
        _isrunning = true;
        struct sockaddr_in peer;
        socklen_t len = sizeof(peer);
        while(_isrunning)
        {
            char buffer[1024];
            // 1.收消息；
            ssize_t s = recvfrom(_sockfd, buffer, sizeof(buffer)-1, 0, (struct sockaddr*)&peer, &len);
            if(s > 0)
            {
                buffer[s] = 0;
                LOG(LogLevel::DEBUG) << "buffer: " << buffer;
                // 2. 发消息:
                std::string echo_string = "server echo@ ";
                echo_string += buffer;
                // .c_str()：C++ 中用于将 std::string 类型转换为 C 风格字符串
                sendto(_sockfd, echo_string.c_str(), echo_string.size(), 0, (struct sockaddr*)&peer, len);
            }
        }
    }

    ~UdpServer()
    {}
private:
    int _sockfd;
    /// uint16_t是无符号整数
    uint16_t _port;
    std::string _ip;// 用的是字符串类型，点分十进制(eg:193.168.0.1)
    bool _isrunning;
};