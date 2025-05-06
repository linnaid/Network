#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>

int main()
{
    char buffer[1024];
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0)
    {
        perror("Socket_Server Creat Failed");
        return -1;
    }
    struct sockaddr_in fd;
    fd.sin_family = AF_INET;
    fd.sin_port = htons(8080);
    fd.sin_addr.s_addr = INADDR_ANY;
    socklen_t len = sizeof(fd);
    if(bind(sockfd, (sockaddr*)&fd, sizeof(fd)) < 0)
    {
        perror("Bind Failed");
        close(sockfd);
        return -1;
    }
    if(listen(sockfd, SOMAXCONN) < 0)
    {
        perror("Listen Failed");
        close(sockfd);
        return -1;
    }
    struct sockaddr_in cli;
    socklen_t lth = sizeof(cli);
    int f = accept(sockfd, (struct sockaddr*)&cli, &lth);
    if(f < 0)
    {
        perror("accpet errror.");
        close(sockfd);
        return -1;
    }
    
    ssize_t str_len = recv(f, buffer, sizeof(buffer)-1, 0);
    if(str_len < 0)
    {
        perror("recv failed");
        close(sockfd);
        return -1;
    }
    else if(str_len > 0)
    {
        buffer[str_len] = '\0';
        std::cout << "Recive Success! " << std::endl << "The String is: " << buffer << std::endl;
    }

    const char* sev = "I Have Successfully Received Data Via TCP";
    ssize_t dec = send(f, sev, strlen(sev), 0);
    if(dec < 0)
    {
        perror("Send Failed---");
    }
    close(f);
    close(sockfd);
    return 0;
}