#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>

#define NUM 8080

int intal = -1;

class Server
{
public:
    Server(uint16_t port)
    :_port(port),
    sockfd(intal)
    {}

    void init()
    {
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if(sockfd < 0)
        {
            perror("Socket Creat Failed");
            exit(1);
        }
        struct sockaddr_in se;
        se.sin_addr.s_addr = INADDR_ANY;
        se.sin_family = AF_INET;
        se.sin_port = htons(_port);
        if(bind(sockfd, (struct sockaddr*)&se, sizeof(se)) < 0)
        {
            perror("Bind Failed");
            close(sockfd);
            exit(2);
        }
        if(listen(sockfd, SOMAXCONN) < 0)
        {
            perror("Listen Failed");
            close(sockfd);
            exit(3);
        }
    }

    void start()
    {
        struct sockaddr_in cli;
        socklen_t len = sizeof(cli);
        int f = accept(sockfd, (struct sockaddr*)&cli, &len);
        if(f < 0)
        {
            perror("Accept Failed");
            close(sockfd);
            exit(1);
        }
        while(1)
        {
            
            ssize_t str_len = recv(f, buffer, sizeof(buffer)-1, 0);
            std::string str_buffer(buffer);
            if(str_buffer == "exit")
            {
                std::cout << "Exit Success!" << std::endl;
                close(f);
                break;
            }
            if(str_len < 0)
            {
                perror("Received Failed");
             
                close(f);
                exit(1);
            }
            else if(str_len > 0)
            {
                buffer[str_len] = '\0';
                std::cout << "Recive Success! " << std::endl << "The String is: " << buffer << std::endl;
            }
            
            ssize_t rec = send(f, buffer, strlen(buffer), 0);
            if(rec < 0)
            {
                perror("Send Failed");
                close(f);
              
                exit(1);
            }
        }
    }

    ~Server()
    {
        close(sockfd);
    }

private:
    char buffer[1024];
    int sockfd;
    uint16_t _port;
    std::string _ip;
};