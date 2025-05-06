#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>


int main()
{
    const char* Server = "127.0.0.1";
    const char* buffer = "I Love You *-*";
    int sockf = socket(AF_INET, SOCK_STREAM, 0);
    if(sockf < 0)
    {
        perror("Socket_Client Creat Failed");
        return -1;
    }
    struct sockaddr_in f;
    if(inet_pton(AF_INET, Server, &f.sin_addr.s_addr) <= 0)
    {
        perror("Invalid Address ~_~");
        close(sockf);
        return -1;
    }
    f.sin_family = AF_INET;
    f.sin_port = htons(8080);
    socklen_t len = sizeof(f);
    
    if(connect(sockf, (struct sockaddr*)&f, len) < 0)
    {
        perror("Connect Failed");
        close(sockf);
        return -1;
    }
    ssize_t dec = send(sockf, buffer, strlen(buffer), 0);
    if(dec < 0)
    {
        perror("Send Failed");
        close(sockf);
        return -1;
    }
    char rece[1024];
    ssize_t re = recv(sockf, rece, sizeof(rece)-1, 0);
    if(re > 0)
    {
        std::cout << "I Received Your Answer! " << std::endl;
    }
    else
    {
        perror("Received Failed");
    }
    close(sockf);
    return 0;
}