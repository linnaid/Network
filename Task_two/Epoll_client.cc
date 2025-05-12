#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

#define NUM 8080
#define IP "127.0.0.1"

int main()
{
    const char* ass = "Hello, I'm Linnaid o-o";
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0)
    {
        perror("Socket Error");
        return -1;
    }
    sockaddr_in s;
    s.sin_family = AF_INET;
    s.sin_port = htons(NUM);
    if(inet_pton(AF_INET, IP, &s.sin_addr.s_addr) < 0)
    {
        perror("Inet_pton Error");
        close(sockfd);
        return -1;
    }
    socklen_t len = sizeof(s);
    if(connect(sockfd, (sockaddr*)&s, len) < 0)
    {
        perror("Connect Error");
        close(sockfd);
        return -1;
    }

    if(write(sockfd, ass, strlen(ass)) < 0)
    {
        perror("Write Error");
        close(sockfd);
    }
    std::cout << "Send Success" << std::endl;
    close(sockfd);
    return 0;
}