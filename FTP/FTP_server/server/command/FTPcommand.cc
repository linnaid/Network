#include "FTPcommand.hpp"
#include <pwd.h>
#include <grp.h>


void command::cmd_execute(FTPconnect& conn, const std::string cmd_line)
{
    std::istringstream iss(cmd_line);
    std::string cmd_t, cmd_m;
    iss >> cmd_t >> cmd_m;
    std::string name;
    if(cmd_t == "USER")
    USER(conn, cmd_m);
    else if(cmd_t == "PASS")
    PASS(conn, cmd_m);
    else if(cmd_t == "STOR")
    STOR(conn, cmd_m);
    else if(cmd_t == "RETR")
    RETR(conn, cmd_m);
    else if(cmd_t == "LIST")
    LIST(conn, cmd_m);
    else if(cmd_t == "QUIT")
    QUIT(conn);
    else
    conn.send_response("502 Command not implemented.");

}

bool command::Login(FTPconnect& conn)
{
    if(conn.login)
    return true;
    else
    {
        return false;
    }
}

void command::LIST(FTPconnect& conn, const std::string& path)
{
    if(!Login(conn)) return;
    std::string _path = path.empty() ? conn.get_current_path() : path;
    DIR* dir = opendir(_path.c_str());
    if(!dir){
        conn.send_response("550 Failed to open directory.");
        return;
    }
    conn.send_response("150 Here comes the directory listing.");
    std::ostringstream l;
    struct dirent* dir_fd;
    while((dir_fd = readdir(dir)) != nullptr)
    {
        std::string every_fd = _path + "/" + dir_fd->d_name;
        struct stat st;
        if(stat(every_fd.c_str(), &st) == -1) continue;
        l << (S_ISDIR(st.st_mode) ? "d" : "-");
        l << ((st.st_mode & S_IRUSR) ? "r" : "-");
        l << ((st.st_mode & S_IWUSR) ? "w" : "-");
        l << ((st.st_mode & S_IXUSR) ? "x" : "-");
        l << ((st.st_mode & S_IRGRP) ? "r" : "-");
        l << ((st.st_mode & S_IWGRP) ? "w" : "-");
        l << ((st.st_mode & S_IXGRP) ? "x" : "-");
        l << ((st.st_mode & S_IROTH) ? "r" : "-");
        l << ((st.st_mode & S_IWOTH) ? "w" : "-");
        l << ((st.st_mode & S_IXOTH) ? "x" : "-");
        l << " 1 ";

        struct passwd* pw = getpwuid(st.st_uid);
        struct group* gp = getgrgid(st.st_gid);
        l << (pw ? pw->pw_name : "unknown") << " ";
        l << (gp ? gp->gr_name : "unknown") << " ";
        l << st.st_size << " ";

        char time[48];
        strftime(time, sizeof(time), "%b %d %H:%M", localtime(&st.st_mtime));
        l << time << " ";

        l << dir_fd->d_name << "\r\n";
    }   
    closedir(dir);

    conn.data_conn->send_data(l.str());

}

void command::RETR(FTPconnect& conn, const std::string& path)
{
    if(!Login(conn)) return;
    conn.data_conn->send_file(path, conn);
}

void command::STOR(FTPconnect& conn, const std::string& path)
{
    if(!Login(conn)) return;
    conn.data_conn->recive_file(path, conn);
}

void command::USER(FTPconnect& conn,const std::string& name)
{
    conn.set_username(name);
    conn.send_response("331 User name okay, need password.");
}

void command::PASS(FTPconnect& conn, const std::string& pass)
{
    std::string _name = conn.get_username();
    if(conn.is_login_in(_name, pass))
    conn.send_response("230 User logged in.");
    else
    conn.send_response("530 Not logged in.");
}

void command::QUIT(FTPconnect& conn)
{
    conn.send_response("221 Goodbye.");
    std::cout << "goodbye~" << std::endl;
    conn.Close();
}