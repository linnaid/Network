#include <iostream>
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <arpa/inet.h>

#define NUM 8080
#define MAX 5
#define SERVE "127.0.0.1"

int main()
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0)
    {
        perror("Socket Error");
        return -1;
    }
    sockaddr_in cli;
    cli.sin_port = htons(NUM);
    cli.sin_family = AF_INET;
    if(inet_pton(AF_INET, SERVE, &cli.sin_addr.s_addr) <= 0)
    {
        perror("Invalid Address");
        close(sockfd);
        return -1;
    }
    socklen_t len = sizeof(cli);

    if(connect(sockfd, (sockaddr*)&cli, len) < 0)
    {
        perror("Connect Error");
        close(sockfd);
        return -1;
    }

    std::cout << "Connect Success!" << std::endl;

    return 0;
}