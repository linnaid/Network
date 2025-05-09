
#include <iostream>
#include <memory>
#include  "UdpServer.hpp"


// 通过"./udpserver ip port"启动服务器
int main(int argc, char *argv[])
{
    // argv[0] 是程序的名称，argv[1] 是 ip，argv[2] 是 port；
    if(argc != 3)
    {
        std::cerr << "Usage: " << argv[0] << " ip port" << std::endl;
        return 1;
    }
    std::string ip = argv[1];
    // std::stoi: C++ 中用于将字符串转换为整数
    uint16_t port = std::stoi(argv[2]);
    Enable_Console_Log_Strategy();

    std::unique_ptr<UdpServer> usvr = std::make_unique<UdpServer>(ip, port);
    usvr->init();
    usvr->start();
    return 0;
    
}