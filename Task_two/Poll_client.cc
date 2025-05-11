#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <poll.h>
#include <arpa/inet.h>
#include <string.h>

#define NUM 8080
#define SUM 5
#define ADD "127.0.0.1"

int main()
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0)
    {
        perror("Socket Error");
        return -1;
    }
    sockaddr_in fd;
    if(inet_pton(AF_INET, ADD, &fd.sin_addr.s_addr) <= 0)
    {
        perror("Invalid Address");
        close(sockfd);
        return -1;
    }
    fd.sin_family = AF_INET;
    fd.sin_port = htons(NUM);
    socklen_t len = sizeof(fd);

    const char* ass = "Hello!";

    if(connect(sockfd, (sockaddr*)&fd, len) < 0)
    {
        perror("connect Error");
        close(sockfd);
        return -1;
    }

    ssize_t a = write(sockfd, ass, strlen(ass));
    if(a < 0)
    {
        perror("Write Error");
        close(sockfd);
        return -1;
    }

    close(sockfd);

    return 0;
}