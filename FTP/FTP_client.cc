#include "FTP_client.hpp"

#define IP "127.0.0.1"
#define PORT 2100

int t = -1;

FTPClient::FTPClient(uint16_t port)
        :sockfd(t),
        _port(PORT)
        {}

void FTPClient::init()
{
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0)
    {
        perror("Sockfd Error");
        exit(1);
    }
    bzero(&cli, sizeof(cli));
    cli.sin_family = AF_INET;
    cli.sin_port = htons(_port);
    inet_pton(AF_INET, _ip.c_str(), &cli.sin_addr.s_addr);
    // if(bind(sockfd, (sockaddr*)&cli, sizeof(cli)) < 0)
    // {
    //     perror("Bind Error");
    //     close(sockfd);
    //     exit(1);
    // }

}

FTPClient::~FTPClient()
{
    if(sockfd >= 0)
    {
        close(sockfd);
    }
}

void FTPClient::start()
{
    Connect();
    bool a = true;
    while(a)
    {
        a = Send();
    }

}

void FTPClient::Connect()
{
    socklen_t l = sizeof(cli);
    if(connect(sockfd, (sockaddr*)&cli, l) < 0)
    {
        perror("Connect Error");
        close(sockfd);
        return;
    }
}

bool FTPClient::Send()
{
    char buf[1024];
    std::cin >> buf;
    if(send(sockfd, buf, strlen(buf), 0) < 0)
    {
        perror("Send_string Error");
        return false;
    }
    if(send(sockfd, "\r\n", 2, 0) < 0)
    {
        perror("Send \r\n Error");
        return false;
    }
    if(strcmp(buf, "QUIT") == 0)
    {
        return false;
    }
    return true;
}

int main()
{
    FTPClient Client(PORT);
    Client.init();
    Client.start();
    return 0;
}