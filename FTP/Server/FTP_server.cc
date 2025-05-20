#include "FTP_server.hpp"
#include <sys/stat.h>
#include <linux/stat.h>

#define SUM 5
#define MAX_NUM 1024

int port = 2100;
std::vector<char> buf(MAX_NUM);
std::vector<char> lin(MAX_NUM);
int s = 0;

void peimission(mode_t mode)
{
    char p[11] = "----------";
    if(S_ISDIR(mode))
    p[0] = 'd';
    if(mode & S_IRUSR)
    p[1] = 'r';
}

FTPServer::~FTPServer()
{
    if(sockfd >= 0)
    close(sockfd);
    if(sock_lis >= 0)
    close(sock_lis);
}

void FTPServer::start()
{
    while(true)
    {
        sockaddr_in cli;
        socklen_t len = sizeof(cli);
        int cli_fd = accept(sockfd, (sockaddr*)&cli, &len);
        if(cli_fd < 0)
        {
            perror("Accept Error");
            continue;
        }
        else
        {
            std::cout << " Hello!" << std::endl << std::endl;
        }

        Threads.push_back(std::thread(&FTPServer::Task, this, cli_fd));

    }
    for(auto &T : Threads)
    {
        T.join();
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

void FTPServer::Task(int cli_fd)
{
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
            
        }
        // std::cout << buf << std::endl;
        if(strncmp(buf.data(), "PASV", 4) == 0)
        {
            n = 1;
            s = 1;
            Send_P(cli_fd, &Data_fd);
        }
        if(n == 0)
        {
            std::cout << "I Recived, The String is: " << buf.data() << std::endl;
        }
        
    }
    close(cli_fd);
}

bool FTPServer::CommandHandler(char* buf, int cli_fd, int *Data_fd)
{
    lin.clear();
    lin.resize(1024);
    std::string str;
    // std::cout << buf << std::endl;
    if(strncmp(buf, "USER ", 5) == 0)
    {
        str = "331  Please enter your password.\r\n";
        lin.assign(str.begin(), str.end());
        lin.push_back('\0');
    }
    else if(strncmp(buf, "PASS ", 5) == 0)
    {
        str = "230 Login successful.\r\n";
        lin.assign(str.begin(), str.end());
        lin.push_back('\0');
    }
    else if(strncmp(buf, "RETR ", 5) == 0)
    {
        char file[MAX_NUM];
        strncpy(file, buf + 5, sizeof(file) - 1);
        std::cout << file << std::endl;
        std::ifstream file_read(file, std::ios::binary);
        if(!file_read)
        {
            perror("READ_FILE ERROR");
            Send(cli_fd, "550 File not found.\r\n");
        }
        Send(cli_fd, "150 Opening BINARY mode data connection...\r\n");
        while(file_read)
        {
            file_read.read(file, sizeof(file));
            std::streamsize size = file_read.gcount();
            if(size > 0)
            {
                ssize_t len = send(*Data_fd, file, strlen(file), 0);
                if(len < 0)
                {
                    perror("Send_file Error");
                    break;
                }
            }
        }
        Send(cli_fd, "226 Transfer complete.\r\n");
    }
    else if(strncmp(buf, "STOR ", 5) == 0)
    {
        STOR(buf, cli_fd, Data_fd);
        return true;
    }
    else if(strncmp(buf, "PWD ", 4) == 0)
    {
        str = " /home/user is the current directory.\r\n";
        lin.assign(str.begin(), str.end());
        lin.push_back('\0');
    }
    else if(strncmp(buf, "LIST ", 5) == 0 || strcmp(buf, "LIST\r\n") == 0)
    {
        List();
        close(*Data_fd);
    }
    else
    {
        Send(cli_fd, "500 Syntax error, command unrecognized.");
        return false;
    }
    return false;
}

std::ofstream FTPServer::File_creat(char* buf)
{
    int len = strlen(buf);
    char fd[MAX_NUM];
    strncpy(fd, buf + 5, sizeof(fd) - 1);
    fd[len - 5] = '\0';
    // std::cout << fd << std::endl;
    std::filesystem::path fdpath(fd);
    std::string fdname = fdpath.filename().string();
    std::ofstream open_file(fdname, std::ios::binary);
    if(!open_file)
    {
        perror("OPEN_file Error");
    }
    return open_file;
}

void FTPServer::STOR(char* buf, int cli_fd, int *Data_fd)
{
    std::ofstream file = File_creat(buf);
    if(!file)
    {
        Send(cli_fd, "550 Requested action not taken. File unavailable\r\n");
        close(sock_lis);
        return;
    }
    else
    Send(cli_fd, "150 Opening BINARY mode data connection for filename.txt.\r\n");
        // std::cout << "233" << std::endl;
    while(1)
    {
        char asss[MAX_NUM];
        ssize_t k = recv(*Data_fd, asss, sizeof(asss) - 1, 0);
        if(k < 0)
        {
            perror("Recv Error");
            break;
        }
        else if(k == 0)
        {
            std::cout << "Client closed connection." << std::endl;
            close(*Data_fd);
            break;
        }
        asss[k] = '\0';
        // std::cout << *Data_fd << std::endl;
        file.write(asss, sizeof(asss));
        // std::cout << asss << std::endl;
    }
    Send(cli_fd, "226 Transfer complete\r\n");
    close(sock_lis);
}

void FTPServer::Send(int cli_fd, const std::string &str)
{
    // if(s == 2)
    // {
        if(send(cli_fd, str.c_str(), str.size(), 0) < 0)
        {
            perror("Send Error");
            //close(cli_fd);
            return;
        }
    // }
}

void FTPServer::List()
{

}

void FTPServer::Send_P(int cli_fd, int *Data_fd)
{
    int p1, p2;
    sock_lis = socket(AF_INET, SOCK_STREAM, 0);
    if(sock_lis < 0)
    {
        perror("Lis_Sock Error");
        return;
    }
    sockaddr_in lis{};
    lis.sin_family = AF_INET;
    // inet_pton(AF_INET, "192.168.1.1", &lis.sin_addr.s_addr);
    //lis.sin_addr.s_addr = INADDR_ANY;
    lis.sin_port = 0;
    bind(sock_lis, (sockaddr*)&lis, sizeof(lis));
    listen(sock_lis, SOMAXCONN);
    //
    sockaddr_in artul_lis;
    socklen_t len = sizeof(artul_lis);
    getsockname(sock_lis, (sockaddr*)&artul_lis, &len);
    //
    sockaddr_in local_ip;
    socklen_t len2 = sizeof(local_ip);
    getsockname(cli_fd, (sockaddr*)&local_ip, &len2);
    char adr[MAX_NUM];
    inet_ntop(AF_INET, &local_ip.sin_addr, adr, sizeof(adr));
    //std::cout << adr << std::endl;
    int ip1,ip2,ip3,ip4;
    sscanf(adr, "%d.%d.%d.%d", &ip1, &ip2, &ip3, &ip4);
    uint16_t port = ntohs(artul_lis.sin_port);
    // std::cout << port << std::endl;
    p1 = port / 256;
    p2 = port % 256;
    //std::cout << port << std::endl;
    snprintf(_str, sizeof(_str) - 1, "227 Entering Passive Mode (%d,%d,%d,%d,%d,%d)\r\n", ip1, ip2, ip3, ip4, p1, p2);
    if(send(cli_fd, _str, strlen(_str), 0) < 0)
    {
        perror("PASV recv Error");
        return;
    }
    *Data_fd = accept(sock_lis, nullptr, nullptr);
    if(*Data_fd < 0)
    {
        perror("Data_fd-Created Error");
    }
    //std::cout << Data_fd << std::endl;
}

int FTPServer::Recive(int cli_fd, char* buf)
{
    int a = recv(cli_fd, buf, MAX_NUM - 1, 0);
    if(a < 0)
    {
        perror("Recive Error");
        //close(cli_fd);
        return a;
    }
    else
    {
        buf[a] = '\0';
    }
    return a;
}

int main()
{
    FTPServer Server(2100);
    Server.init();
    Server.start();

    return 0;
}

