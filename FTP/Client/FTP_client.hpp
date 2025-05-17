#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>
#include <vector>
#include <thread>
#include <arpa/inet.h>


class FTPClient
{
public:
    FTPClient(uint16_t port);
    void init();
    void start();
    ~FTPClient();
private:
    bool Send(int sockfd);
    void Connect(int sockfd, sockaddr_in cli);
    bool Recive();
    void Decide();
    void Trun(int a);

    int _sockfd;
    int link_sock;
    std::string _ip;
    int _port;
    sockaddr_in _cli;
    int n = 0;

};