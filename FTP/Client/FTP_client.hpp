#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>
#include <vector>
#include <thread>
#include <arpa/inet.h>
#include <fstream>
#include <filesystem>

#define IP "127.0.0.1"
#define PORT 2100
#define MAX_NUM 1024

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
    void Decide(bool b);
    void Trun(int a);
    void List();
    void Stor();
    void Retr();
    std::ofstream File_creat(char* buf);

    int _sockfd;
    int link_sock;
    std::string _ip;
    int _port;
    sockaddr_in _cli;
    int n = 0;

};