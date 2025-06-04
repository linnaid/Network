#pragma once

#include "include.hpp"
#include "../connect/FTPconnection.hpp"
// #include "json.hpp"

class FTPconnect;

class command{
public:
    void cmd_execute(FTPconnect& conn, const std::string cmd_line);
    
private:
    static void STOR(FTPconnect& conn, const std::string& path);
    static void RETR(FTPconnect& conn, const std::string& path);
    static void USER(FTPconnect& conn, const std::string& name);
    static void PASS(FTPconnect& conn, const std::string& pass);
    static void LIST(FTPconnect& conn, const std::string& path);
    static void QUIT(FTPconnect& conn);
    static bool Login(FTPconnect& conn);
};