#pragma once

#include "include.hpp"
#include "../connect/FTPconnection.hpp"

class FTPconnect;

class Dataconnect{
public:
    void send_passive(const int& cli_fd, FTPconnect& conn);
    void accept_data();
    void send_file(const std::string& path, FTPconnect& conn);
    void recive_file(const std::string& path, FTPconnect& conn);
    void Close();  
    void send_data(const std::string& _data);
    
private:
    int make_nonblocking(int fd); 
    std::ofstream file_creat(const std::string& path);
    
    int listen_pasv;
    int data_fd;
};