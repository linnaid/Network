#include "FTP_server.hpp"


#define SUM 5
#define MAX_NUM 1024

int port = 2100;
std::vector<char> buf(MAX_NUM);
std::vector<char> lin(MAX_NUM);
int s = 0;

FTPServer::~FTPServer()
{
    if(sockfd >= 0)
    close(sockfd);
    if(sock_lis >= 0)
    close(sock_lis);
    if(main_epoll >= 0)
    close(main_epoll);
}

void FTPServer::start()
{
    if(make_nonblocking(sockfd) == -1)
    {
        perror("Make_Nonblocking ERROR");
        exit(1);
    }
    main_epoll = epoll_create1(0);
    struct epoll_event ev{};
    ev.data.fd = sockfd;
    ev.events = EPOLLIN;
    if(epoll_ctl(main_epoll, EPOLL_CTL_ADD, sockfd, &ev) < 0)
    {
        perror("epoll_ctl ERROR");
        exit(1);
    }
    while(true)
    {
        int n = epoll_wait(main_epoll, _events, SUM, _time);
        for(int i = 0; i < n; i++)
        {
            int fd = _events[i].data.fd;
            if(fd == sockfd)
            {
                sockaddr_in cli;
                socklen_t len = sizeof(cli);
                int cli_fd = accept(sockfd, (sockaddr*)&cli, &len);
                if(cli_fd < 0)
                {
                    perror("Accept Error");
                    std::cout << "sss" << std::endl;
                    continue;
                }
                else
                {
                    std::cout << " Hello!" << std::endl;
                }
                struct epoll_event event;
                event.data.fd = cli_fd;
                event.events = EPOLLIN;
                make_nonblocking(cli_fd);
                epoll_ctl(main_epoll, EPOLL_CTL_ADD, cli_fd, &event);
            }
            else
            {
                
            }
        }
    }
      
}

void FTPServer::init()
{
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0)
    {
        perror("Socket Error");
        exit(1);
    }
    memset(&_fd, 0, sizeof(_fd));
    _fd.sin_addr.s_addr = INADDR_ANY;
    _fd.sin_port = htons(_port);
    _fd.sin_family = AF_INET;
    if(bind(sockfd, (sockaddr*)&_fd, sizeof(_fd)) < 0)
    {
        perror("Bind Error");
        exit(1);
    }
    if(listen(sockfd, SOMAXCONN) < 0)
    {
        perror("Listen Error");
        exit(1);
    }
}

int FTPServer::make_nonblocking(int sockfd)
{
    int sock = fcntl(sockfd, F_GETFL);
    if(sock == -1)
    {
        return -1;
    }
    if(fcntl(sockfd, F_SETFL, sock | O_NONBLOCK) == -1)
    {
        return -1;
    }
    return 0;

}

void FTPServer::Task(int cli_fd)
{
    // if(make_blocking(cli_fd) == -1)
    // {
    //     perror("Make_block ERROR");
    //     return;
    // }
    int Data_fd = -1;
    n = 0;
    while(true)
    {
        // std::cout << "233" << std::endl;
        // Connect(cli_fd);
        int a = Recive(cli_fd, buf.data());
        if(a <= 0)
        {
            break;
        }
        if(a > 0)
        buf[a] = '\0';
        // int c = strlen(buf);
        // std::cout << Data_fd << std::endl;
        if(strncmp(buf.data(), "QUIT", 4) == 0)
        {
            std::cout << " OK, bye bye~ " << std::endl;
        }
        if(n == 1)
        {
            if(CommandHandler(buf.data(), cli_fd, &Data_fd))
            n = 0;
            // std::cout << "233" << std::endl;
            // std::cout << n << std::endl;
        }
        // std::cout << n << std::endl;
        // std::cout << buf << std::endl;
        if(strncmp(buf.data(), "PASV", 4) == 0 && n == 0)
        {
            n = 1;
            s = 1;
            Send_P(cli_fd, &Data_fd);
            // std::cout << n << "+1" << std::endl;
        }
        if(n == 0 && a > 0)
        {
            std::cout << "I Recived, The String is: " << buf.data() << std::endl;
        }
        
    }
    close(cli_fd);
}
