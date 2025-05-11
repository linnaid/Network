#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <poll.h>

#define NUM 8080
#define SUM 5


int main()
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0)
    {
        perror("Socket Error");
        return -1;
    }
    sockaddr_in fd;
    fd.sin_addr.s_addr = INADDR_ANY;
    fd.sin_family = AF_INET;
    fd.sin_port = htons(NUM);
    socklen_t len = sizeof(fd);
    if(bind(sockfd, (struct sockaddr*)&fd, len) < 0)
    {
        perror("Bind Error");
        close(sockfd);
        return -1;
    }
    if(listen(sockfd, SUM) < 0)
    {
        perror("Listen Error");
        close(sockfd);
        return -1;
    }

    struct pollfd file[SUM + 1];
    file[0].events = POLLIN;
    file[0].fd = sockfd;
    int nfd = 1;
    int time = 4000;
    int n = 0;

    while(true)
    {
        int lis = poll(file, nfd, time);
        if(lis < 0)
        {
            perror("Poll Error");
            close(file[0].fd);
            return -1;
        }
        else if(lis == 0)
        {
            std::cout << "Poll timeout, no events occurred within 4 seconds" << std::endl;
            n++;
            if(n == SUM)
            {
                break;
            }
            continue;
        }
        else
        {
            int acc = accept(file[0].fd, nullptr, nullptr);
            if(acc < 0)
            {
                perror("Accept Error");
                continue;
            }
            else
            {
                //
                if(nfd < SUM +1)
                {
                    file[nfd].fd = acc;
                    file[nfd].events = POLLIN;
                    nfd++;
                }
                else
                {
                    std::cerr << "Too many clients, connection rejected" << std::endl;
                    close(acc);
                }
            }
        }

        for(int i = 1; i < nfd; i++)
        {
            if(file[i].revents & POLLIN)
            {
                char buf[1024];
                int n = read(file[i].fd, buf, sizeof(buf) - 1);
                if(n <= 0)
                {
                    std::cout << "Close Client Link" << std::endl;
                    close(file[i].fd);
                    file[i] = file[nfd - 1];
                    nfd--;
                    i--;
                }
                else
                {
                    buf[n] = '\0';
                    std::cout << "Recive Success! The String Is: " << buf << std::endl;
                }
            }
        }
    }
    for(int i = 0; i < nfd; i++)
    {
        close(file[i].fd);
    }

    return 0;
}