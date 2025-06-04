#include "Dataconnect.hpp"

void Dataconnect::send_passive(const int& cli_fd, FTPconnect& conn)
{
    int p1, p2;
    listen_pasv = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in sock_pasv;
    sock_pasv.sin_family = AF_INET;
    sock_pasv.sin_port = 0; 
    bind(listen_pasv, (sockaddr*)&sock_pasv, sizeof(sock_pasv));
    listen(listen_pasv, SOMAXCONN);
    if(make_nonblocking(listen_pasv) == -1){
        perror("LISTEN_PASV MAKE_NONBLOCK ERROR");
        return;
    }
    sockaddr_in artual_port;
    socklen_t len1 = sizeof(artual_port);
    getsockname(listen_pasv, (sockaddr*)&artual_port, &len1);
    sockaddr_in local_ip;
    socklen_t len2 = sizeof(local_ip);
    getsockname(cli_fd, (sockaddr*)&local_ip, &len2);
    char buf[1024];
    inet_ntop(AF_INET, &local_ip.sin_addr.s_addr, buf, sizeof(buf));
    int ip1, ip2, ip3, ip4;
    char dot;
    std::istringstream iss(buf);
    iss >> ip1 >> dot >> ip2 >> dot >> ip3 >> dot >> ip4;
    uint16_t _port = ntohs(artual_port.sin_port);
    p1 = _port / 256;
    p2 = _port % 256;
    std::ostringstream oss;
    oss << "227 Entering Passive Mode (" << ip1 << "," << ip2 << "," << ip3 << "," << ip4 << "," << p1 << "," << p2 << ")";
    std::string ass = oss.str();
    conn.send_response(ass);
}

void Dataconnect::accept_data()
{
    data_fd = accept(listen_pasv, nullptr, nullptr);
//     if(data_fd < 0)
//     perror("ACCEPT_DATA ERROR");
    std::cout << "PASV Openning" << std::endl;
    if(make_nonblocking(data_fd) == -1){
        perror("DATA_FD MAKE_NONBLOCK ERROR");
        return;
    }
}

void Dataconnect::send_file(const std::string& path, FTPconnect& conn)
{
    std::ifstream file(path, std::ios::binary);
    if(!file){
        perror("OPEN_FILE ERROR");
        conn.send_response("550 File not found.");
    }
    char test[1024];
    conn.send_response("150 Opening BINARY mode data connection...");
    while(file)
    {
        file.read(test, sizeof(test));
        std::streamsize size = file.gcount();
        if(size > 0)
        {
            ssize_t len = send(listen_pasv, test, size, 0);
            if(len < 0){
                // perror("SEND ERROR");
                break;
            }
        }
    }
    file.close();
    Close();
    conn.send_response("226 Transfer complete.");
}

void Dataconnect::recive_file(const std::string& path, FTPconnect& conn)
{
    std::ofstream f_open = file_creat(path);
    if(!f_open){
        perror("FILE_CREAT ERROR");
        conn.send_response("550 Requested action not taken. File unavailable");
        Close();
        return;
    }
    conn.send_response("150 Opening BINARY mode data connection for filename.txt.");
    char buf[1024];
    while(1)
    {
        ssize_t l = recv(data_fd, buf, sizeof(buf), 0);
        if(l > 0)  f_open.write(buf, l);
        else if(l == 0) {
            std::cout << "Client closed connection." << std::endl;
            break;
        }
        else {
            if(errno == EAGAIN || errno == EWOULDBLOCK){
                continue;
            }
            else{
                perror("RECV_FILE ERROR");
                break;
            }
            
        }
    }
    Close();
    f_open.close();
    conn.send_response("226 Transfer complete");
}

int Dataconnect::make_nonblocking(int fd)
{
    int flag = fcntl(fd, F_GETFL);
    if(flag == -1) return -1;
    return fcntl(fd, F_SETFL, flag | O_NONBLOCK);
}

void Dataconnect::Close()
{
    close(data_fd);
    close(listen_pasv);
}

std::ofstream Dataconnect::file_creat(const std::string& path)
{
    std::string f_path = "/home/linnaid/Task/Network/FTP/FTP_server";
    std::filesystem::path f_name(path);
    std::string f = f_name.filename().string();
    std::filesystem::path fdname = std::filesystem::path(f_path) / f;
    std::ofstream f_open(fdname, std::ios::binary);
    return f_open;
}

void Dataconnect::send_data(const std::string& _data)
{
    if(send(data_fd, _data.c_str(), _data.size(), 0) < 0){
        perror("SEND_DATA ERROR");
    }
    Close();
}
