#include "FTP_client.hpp"



int t = -1;
std::vector<char> buf(MAX_NUM);
std::vector<char> arr(MAX_NUM);
std::vector<char> ass(MAX_NUM);
int p1 = 0 ,p2 = 0;

FTPClient::FTPClient(uint16_t port)
        :_sockfd(t),
        link_sock(t),
        _port(PORT)
        {}

void FTPClient::init()
{
    _sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(_sockfd < 0)
    {
        perror("Sockfd Error");
        exit(1);
    }
    bzero(&_cli, sizeof(_cli));
    _cli.sin_family = AF_INET;
    _cli.sin_port = htons(_port);
    inet_pton(AF_INET, _ip.c_str(), &_cli.sin_addr.s_addr);
    // if(bind(sockfd, (sockaddr*)&cli, sizeof(cli)) < 0)
    // {
    //     perror("Bind Error");
    //     close(sockfd);
    //     exit(1);
    // }

}

FTPClient::~FTPClient()
{
    if(_sockfd >= 0)
    {
        close(_sockfd);
    }
    if(link_sock >= 0)
    {
        close(link_sock);
    }
}


int _send = 0;
void FTPClient::start()
{
    Connect(_sockfd, _cli);
    bool a = true;
    bool b = false;
    while(a || b)
    {
        a = Send(_sockfd);
        //std::cout << "www" << std::endl;
        if(strncmp(buf.data(), "QUIT", 4) == 0)
        {
            break;
        }
        if(_send == 1)
        {
            // std::cout << "sss" << std::endl;
            b = Recive();
            
        }
        if(b == true)
        Decide(b);
    }

}

void FTPClient::Decide(bool b)
{
    // std::cout << arr.data() << std::endl;
    std::string e = "error\0";
    int a = strlen(arr.data());
    // std::cout << a << std::endl;
    if(strncmp(arr.data(), "227", 3) == 0)
    {
        std::cout << arr.data() << std::endl;
        Trun(a);
        // arr.assign(e.begin(), e.end());
    }
    else if(strncmp(arr.data(), "530 ", 4) == 0)
    {
        std::cout << arr.data() << std::endl;
        close(link_sock);
        _send = 0;
        b = false;
    }
    else if(strncmp(arr.data(), "230 ", 4) == 0)
    {
        std::cout << arr.data() << std::endl;
    }
    else if(strncmp(buf.data(), "LIST", 4) == 0)
    {
        List();
        Recive();
        if(strncmp(arr.data(), "226", 3) == 0)
        {
            std::cout << std::endl <<"LIST Command Completed" << std::endl;
        }
    }
    else if(strncmp(arr.data(), "150 Opening BINARY mode data connection...", 42) == 0)
    {
        Retr();
        if(Recive())
        {
            if(strncmp(arr.data(), "226 Transfer complete.", 16) == 0)
            {
                std::cout << "yes" <<std::endl;
                _send = 0;
            }
        }
    }
    else if(strncmp(arr.data(), "150 Opening BINARY mode data connection for filename.txt.", 44) == 0)
    {
        std::cout << arr.data() <<std::endl;
        Stor();
        if(n == -1)
        {
            n = 0;
            return;
        }
        if(Recive())
        {
            if(strncmp(arr.data(), "226 Transfer complete", 16) == 0)
            {
                // close(link_sock);
                std::cout << "yes" << std::endl;
                _send = 0;
            }
        }
    }
    else if(strncmp(arr.data(), "500", 3) == 0)
    {
        std::cout << arr.data() <<std::endl;
        b = false;
    }
    else if(strncmp(arr.data(), "550", 3) == 0)
    {
        std::cout << "OPEN_FILE ERROR" << std::endl;
        b = false;
    }
}

std::ofstream FTPClient::File_creat(char* buf)
{
    int len = strlen(buf);
    char fd[MAX_NUM];
    strncpy(fd, buf + 5, sizeof(fd) - 1);
    fd[len - 5] = '\0';
    // std::cout << fd << std::endl;
    std::filesystem::path fdpath(fd);
    std::string path_ = "/home/linnaid/Task/Network/FTP/Client";
    std::string fdname = fdpath.filename().string();
    std::filesystem::path f_path = std::filesystem::path(path_) / fdname;
    std::ofstream open_file(f_path, std::ios::binary);
    if(!open_file)
    {
        perror("OPEN_file Error");
    }
    return open_file;
}

void FTPClient::Retr()
{
    std::ofstream file = File_creat(buf.data());
    if(!file)
    {
        perror("OPEN_FILE ERROR");
        close(link_sock);
    }
    while(1)
    {
        char re[MAX_NUM];
        ssize_t k = recv(link_sock, re, sizeof(re) - 1, 0);
        if(k < 0)
        {
            perror("RETR_RECIVE ERROR");
            break;
        }
        else if(k == 0)
        {
            // std::cout << "Server closed connection." << std::endl;
            break;
        }
        // std::cout << re << std::endl;
        else file.write(re, k);
    }
    // std::cout << "lll" << std::endl;
    close(link_sock);
}

void FTPClient::Stor()
{
    for(int i = 0; i < buf.size(); i++)
    {
        if(buf[i] == ' ')
        {
            // std::cout << buf.data() << std::endl;
            ass.assign(buf.begin() + 5, buf.end());
            std::cout << "Path to open: [" << ass.data() << "]" << std::endl;
            std::ifstream file(ass.data(), std::ios::binary);
            if(!file)
            {
                perror("Failed to open file");
                n = -1;
                return;
            }
            ass.clear();
            ass.resize(1024);
            while(file)
            {
                file.read(ass.data(), ass.size());
                // std::cout << ass.data() << std::endl;
                std::streamsize size = file.gcount();
                if(size > 0)
                {
                    ssize_t sen = send(link_sock, ass.data(), size, 0);
                    std::cout << sen << std::endl;
                    if(sen < 0)
                    {
                        perror("File_Send Error");
                        break;
                    }
                }
                // std::cout << "sss" << std::endl;
            }
            close(link_sock);
            break;
        }
    }
}

void FTPClient::List()
{
    char lis[MAX_NUM];
    while(1)
    {
        ssize_t lis_l = recv(link_sock, lis, sizeof(lis) - 1, 0);
        if(lis_l < 0)
        {
            perror("LIST_Recive Error");
            return;
        }
        else if(lis_l == 0)
        break;
        lis[lis_l] = '\0';
        std::cout << lis << std::endl;
    }

}

void FTPClient::Trun(int a)
{
    p1 = 0;
    p2 = 0;
    memset(ass.data(), 0, sizeof(ass));
    // std::cout << a << std::endl;
    for(int i = 0; i < a; i++)
    {
        int n = 0;
        int d = 0;
        //std::cout << arr[i] << std::endl;
        if(arr[i++] == '(')
        {
            //std::cout << a << std::endl;
            while(i < a)
            {
                //std::cout << arr[i] << std::endl;
                if(arr[i] <= '9' && arr[i] >= '0')
                {
                    //std::cout << arr[i] << std::endl;
                    ass[n] = arr[i];
                    n++;
                    i++;
                }
                else if(arr[i] == ',')
                {
                    if(d == 3)
                    {
                        ass[n] = '\0';
                        i++;
                        //std::cout << ass << std::endl;
                        break;
                    }
                    ass[n] = '.';
                    n++;
                    i++;
                    d++;
                }
            }
            //std::cout << ass << std::endl;
            int t = 1;
            while(i < a)
            {
                //std::cout << arr[i] << std::endl;
                if(arr[i] <= '9' && arr[i] >= '0')
                {
                    if(t == 1)
                    {
                        //std::cout << arr[i] << std::endl;
                        p1 = p1 * 10 + (arr[i] - '0'); 
                        i++;
                    }
                    else if(t == 2)
                    {
                        p2 = p2 * 10 + (arr[i] - '0');
                        i++;
                    }
                }
                else if(arr[i] == ',')
                {
                    t = 2;
                    i++;
                }
                
                if(arr[i] == ')')
                break;
            }
        }
        //break;
        //std::cout << arr.data() << std::endl;
    }
    //std::cout << ass << std::endl;
    int p = p1 * 256 + p2;
    // std::cout << ass.data() << std::endl;
    link_sock = socket(AF_INET, SOCK_STREAM, 0);
    if(link_sock < 0)
    {
        perror("Link_Socket Error");
        return;
    }
    sockaddr_in link;
    link.sin_port = htons(p);
    link.sin_family = AF_INET;
    inet_pton(AF_INET, ass.data(), &link.sin_addr.s_addr);
     Connect(link_sock, link);
    std::cout << "success" << std::endl;
}

bool FTPClient::Recive()
{
    int a = recv(_sockfd, arr.data(), arr.size(), 0);
    if(a < 0)
    {
        return false;
    }
    else
    arr[a] = '\0';
    //std::cout << arr.data() << std::endl;
    return true;
}

void FTPClient::Connect(int sockfd, sockaddr_in cli)
{
    socklen_t l = sizeof(cli);
    if(connect(sockfd, (sockaddr*)&cli, l) < 0)
    {
        perror("Connect Error");
        close(sockfd);
        return;
    }
}

bool FTPClient::Send(int sockfd)
{
    buf.clear();
    buf.resize(MAX_NUM);
    std::cin.getline(buf.data(), MAX_NUM);
    int len = strlen(buf.data());
    buf.resize(len);
    buf.push_back('\r');
    buf.push_back('\n');
    // std::string ass = std::string(buf.data()) + "\r\n";
    // buf.insert(buf.end(), {'\r', '\n'});
    // std::cout << buf.data() << std::endl;
    if(send(sockfd, buf.data(), strlen(buf.data()), 0) < 0)
    {
        perror("Send_string Error");
        return false;
    }
    else if(strncmp(buf.data(), "USER", 4) == 0)
    {
        //std::cout << "111" << std::endl;
        buf.pop_back();
        buf.pop_back();
        _send = 1;
        return true;
    }
    buf.pop_back();
    buf.pop_back();
    return true;
}

