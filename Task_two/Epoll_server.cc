#include <iostream>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>

#define NUM 8080
#define SUM 5

int Make_nonblocking(int fd)
{
    int flags = fcntl(fd, F_GETFL);
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

int main()
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0)
    {
        perror("Socket Error");
        return -1;
    }
    Make_nonblocking(sockfd);
    sockaddr_in fd;
    fd.sin_family = AF_INET;
    fd.sin_port = htons(NUM);
    fd.sin_addr.s_addr = INADDR_ANY;
    if(bind(sockfd, (sockaddr*)&fd, sizeof(fd)) < 0)
    {
        perror("Bind Error");
        close(sockfd);
        return -1;
    }
    if(listen(sockfd, SOMAXCONN) < 0)
    {
        perror("Listen Error");
        close(sockfd);
        return -1;
    }

    int time = 3000;
    int a = epoll_create1(0);
    epoll_event ev{}, events[1024]; 
    ev.data.fd = sockfd;
    ev.events = EPOLLIN;
    if(epoll_ctl(a, EPOLL_CTL_ADD, sockfd, &ev) < 0)
    {
        perror("Epoll_Ctl Error");
    }

    while(true)
    {
        int n = epoll_wait(a, events, SUM, time);
        for(int i = 0; i < n; i++)
        {
            int ad = events[i].data.fd;
            if(ad == sockfd)
            {
                sockaddr_in cli;
                socklen_t l = sizeof(cli);
                int ff = accept(sockfd, (sockaddr*)&cli, &l);
                if(ff < 0)
                {
                    perror("Accept Error");
                    continue;
                }
                Make_nonblocking(ff);
                epoll_event cli_e{};
                cli_e.data.fd = ff;
                cli_e.events = EPOLLIN;
                epoll_ctl(a, EPOLL_CTL_ADD, ff, &cli_e);
                std::cout << "Bond Success" << std::endl;
            }
            else
            {
                char buf[1024];
                int arr = read(events[i].data.fd, buf, sizeof(buf) - 1);
                if(arr <= 0)
                {
                    std::cout << "Client disconnected" << std::endl;
                    epoll_ctl(a, EPOLL_CTL_DEL, events[i].data.fd, nullptr);
                    close(events[i].data.fd);
                }
                else
                {
                    buf[arr] = '\0';
                    std::cout << "Recived Success, The String is: " << buf << std::endl;
                }
            }
        }
    }

    close(sockfd);
    close(a);
    
    return 0;
}