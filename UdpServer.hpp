//保证这个头文件只会被包含（编译）一次，防止重复包含导致编译错误
#pragma once

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include "log.hpp"

using namespace LogModule;

const int defaultfd = -1;

class UdpServer
{
public:
    UdpServer():_sockfd(defaultfd)
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
        
    }
    void start();

    ~UdpServer()
    {}
private:
    int _sockfd;
};