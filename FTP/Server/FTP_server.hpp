#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>
#include <vector>
#include <thread>
#include <curl/curl.h>
#include <mutex>
#include <arpa/inet.h>
#include <fstream>
#include <filesystem>
#include <fcntl.h>
#include <sys/epoll.h>
#include "json.hpp"

#define MAX_NUM 1024

int t = -1;

struct User{
    std::string username = "";
    std::string password = "";
};

class FTPServer
{
public:
    FTPServer(uint16_t port)
    :_port(port),
    sockfd(t),
    sock_lis(t),
    _time(timee),
    _epoll(t)
    {}
    void start();
    void init();
    ~FTPServer();

private:
    void Send_P(int cli_fd, int *Data_fd);
    void Task(int cli_fd);
    int make_nonblocking(int sockfd);
    int Recive(int clifd, char* buf);
    bool CommandHandler(char* buf, int cli_fd, int *Data_fd);
    std::vector<User> LoadUsers(const std::string& path);
    bool Login(User user);
    void List();
    void Send(int cli_fd, const std::string &str);
    void STOR(char* buf, int cli_fd, int *Data_fd);
    void RETR(char* buf, int cli_fd, int *Data_fd);
    std::ofstream File_creat(char* buf);
 
    int sockfd;
    int sock_lis;
    int _time;
    int _epoll;
    struct epoll_event _events[MAX_NUM];
    uint16_t _port;
    sockaddr_in _fd;
    std::string ass;
    std::vector<std::thread> Threads;
    std::vector<User> users;
    User user;
    std::mutex _lock;
    char _str[MAX_NUM];
    int n = 0;
    
};