#include <iostream>
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>

#define NUM 8080
#define MAX 5

int main()
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in sockf;
    sockf.sin_port = htons(NUM);
    sockf.sin_family = AF_INET;
    sockf.sin_addr.s_addr = INADDR_ANY;
    socklen_t len = sizeof(sockf);
    if(bind(sockfd, (sockaddr*)&sockf, len) < 0)
    {
        perror("Bind Error");
        close(sockfd);
        return -1;
    }
    if(listen(sockfd, MAX) < 0)
    {
        perror("Listen Error");
        return -1;
    }

    fd_set file;
    int max = sockfd;
    int n = 1;

    while(true)
    {
        FD_ZERO(&file);
        FD_SET(sockfd, &file);

        timeval t;
        t.tv_sec = 3;
        t.tv_usec = 0;

        int acc = select(max + 1, &file, nullptr, nullptr, &t);
        if(acc < 0)
        {
            perror("Select Error");
            break;
        }
        else if(acc == 0)
        {
            std::cout << "No readable data before timeout." << std::endl;
            
            if(n >= MAX)
            {
                break;
            }
            n++;
        }

        if(FD_ISSET(sockfd, &file))
        {
            sockaddr_in cli;
            socklen_t lenth = sizeof(cli);
            int p = accept(sockfd, (struct sockaddr*)&cli, &lenth);
            if(p < 0)
            {
                perror("Accept Error");
                close(p);
                break;
            }
            std::cout << "Accepted a new client connection." << std::endl;
        }
    }

    close(sockfd);

    return 0;
}