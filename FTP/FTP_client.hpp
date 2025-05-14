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
    bool Send();
    void Connect();

    int sockfd;
    std::string _ip;
    int _port;
    sockaddr_in cli;

};