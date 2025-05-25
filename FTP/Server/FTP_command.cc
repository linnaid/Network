#include "FTP_server.hpp"


using json = nlohmann::json;


void peimission(mode_t mode)
{
    char p[11] = "----------";
    if(S_ISDIR(mode))
    p[0] = 'd';
    if(mode & S_IRUSR)
    p[1] = 'r';
}

bool FTPServer::CommandHandler(char* buf, int cli_fd, int *Data_fd)
{
    std::string str;
    // std::cout << buf << std::endl;
    if(strncmp(buf, "USER ", 5) == 0)
    {
        user.username = std::string(buf + 5);
        // std::cout << "ok_name" << std::endl;
        Send(cli_fd, "331  Please enter your password.\r\n");
    }
    else if(strncmp(buf, "PASS ", 5) == 0)
    {
        user.password = std::string(buf + 5);
        if(Login(user))
        {
            Send(cli_fd, "230 User logged in, proceed.\r\n");
            // std::cout << "ok_pass" << std::endl;
        }
        else 
        {
            Send(cli_fd, "530 Login incorrect.\r\n");
            return true;
        }
    }
    else if(strncmp(buf, "RETR ", 5) == 0)
    {
        RETR(buf, cli_fd, Data_fd);
        return true;
    }
    else if(strncmp(buf, "STOR ", 5) == 0)
    {
        STOR(buf, cli_fd, Data_fd);
        return true;
    }
    else if(strncmp(buf, "PWD ", 4) == 0)
    {
        str = " /home/user is the current directory.\r\n";
        
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
    // std::cout << "ssss" << std::endl;
    return false;
}

std::vector<User> FTPServer::LoadUsers(const std::string& path)
{
    std::ifstream file(path);
    json j;
    file >> j;
    std::vector<User> Users;
    for(const auto& item : j["users"])
    {
        User user;
        user.username = item["username"];
        user.password = item["password"];
        Users.push_back(user);
    }

    return Users;
}

bool FTPServer::Login(User user)
{
    users = LoadUsers("/home/linnaid/Task/Network/FTP/config/users.json");
    for(const auto& item: users)
    {
        // std::cout << user.username << std::endl;
        // std::cout << user.password << std::endl;
        if(item.username == user.username && item.password == user.password)
        {

            return true;
        }
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
    std::string path_ = "/home/linnaid/Task/Network/FTP/Server";
    std::filesystem::path fdpath(fd);
    std::string fdname = fdpath.filename().string();
    std::filesystem::path f_path = std::filesystem::path(path_) / fdname;
    std::ofstream open_file(f_path, std::ios::binary);
    if(!open_file)
    {
        perror("OPEN_file Error");
    }
    return open_file;
}

void FTPServer::RETR(char* buf, int cli_fd, int *Data_fd)
{
    char file[MAX_NUM];
    strncpy(file, buf + 5, sizeof(file) - 1);
    // std::cout << file << std::endl;
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
            ssize_t len = send(*Data_fd, file, size, 0);
            if(len < 0)
            {
                perror("Send_file Error");
                break;
            }
        }
    }
    // std::cout << file << std::endl;
    close(*Data_fd);
    close(sock_lis);
    Send(cli_fd, "226 Transfer complete.\r\n");
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
            // if (errno == EAGAIN || errno == EWOULDBLOCK)
            // {
            //     // 非阻塞下，暂时没数据，等下次 epoll 再读
            //     continue;
            // }
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
        file.write(asss, k);
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
            // close(cli_fd);
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
    std::cout << *Data_fd << std::endl;
    std::cout << "PASV openning !" << std::endl;
}

int FTPServer::Recive(int cli_fd, char* buf)
{
    int a = recv(cli_fd, buf, MAX_NUM - 1, 0);
    if(a < 0)
    {
        perror("Recive Error");
        // close(cli_fd);
        return a;
    }
    else
    {
        buf[a] = '\0';
    }
    return a;
}
