#include "FTPconnection.hpp"

using json = nlohmann::json;

FTPconnect::FTPconnect(int cli_fd):
control_fd(cli_fd),
login(false),
data_conn(nullptr),
_pasv(false),
current_dir("/home/linnaid/Task/Network/FTP/FTP_server")
{}

void FTPconnect::process_input(const char* buf)
{
    std::string line = buf;
    size_t start = 0;
    std::lock_guard<std::mutex> lock(comd_mutex);
    std::cout << line << std::endl;
    comds[control_fd] = std::make_shared<command>();
    while(true)
    {
        size_t l = line.find("\r\n", start);
        if(l == std::string::npos) break;
        if(l > start)
        {
            std::string cmd_line = line.substr(start, l - start);
            std::cout << "cmd_line" << std::endl;
            handle_command(cmd_line);
        }
        start = l + 2;
    }
}

void FTPconnect::handle_command(const std::string& cmd_line)
{
    auto it = comds.find(control_fd);
    if(it == comds.end()) { 
        perror("comd_find error");
        return;
    }
    auto comd = it->second;
    if(strncmp(cmd_line.c_str(), "PASV", 4) == 0 && login){
        std::cout << "pasv_wait" << std::endl;
        _pasv = set_pasv_conn();
        return;
    }
    else if(strncmp(cmd_line.c_str(), "USER", 4) == 0){
        comd->cmd_execute(*this, cmd_line);
        return;
    }
    else if(strncmp(cmd_line.c_str(), "PASS", 4) == 0){
        comd->cmd_execute(*this, cmd_line);
        return;
    }
    if(login && _pasv){
        comd->cmd_execute(*this, cmd_line);
        login = false;
        delete data_conn;
        data_conn = nullptr;
    }
    if(!_pasv)
    send_response("425 Can't open data connection.");
    if(!login)
    send_response("530 Please login with USER and PASS.");
}

void FTPconnect::set_username(const std::string& name)
{
    user.username = name;
}

const std::string& FTPconnect::get_username()
{
    return user.username;
}

std::vector<User> FTPconnect::LoadUsers(const std::string path)
{
    std::ifstream file(path);
    json j;
    file >> j;
    std::vector<User> users;
    for(const auto& item : j["users"])
    {
        User user;
        user.username = item["username"];
        user.password = item["password"];
        users.push_back(user);
    }
    return users;
}

bool FTPconnect::is_login_in(const std::string& name, const std::string& pass)
{
    std::lock_guard<std::mutex> lock(user_mutex);
    if(users.empty())
    users = LoadUsers("/home/linnaid/Task/Network/FTP/config/users.json");
    for(const auto& th : users)
    {
        if(name == th.username && pass == th.password)
        {
            login = true;
            return true;
        }
    }
    return false;
}

void FTPconnect::Close()
{
    if(control_fd >= 0)
    close(control_fd);
}

void FTPconnect::send_response(std::string test) const
{
    if(send(control_fd, test.c_str(), test.size(), 0) < 0)
    perror("SEND error");
}

bool FTPconnect::set_pasv_conn()
{
    if(data_conn){
        data_conn->Close();
        delete data_conn;
        data_conn = nullptr;
    }
    
    data_conn = new Dataconnect();

    data_conn->send_passive(control_fd, *this);
    return true;
}

std::string& FTPconnect::get_current_path(){
    return current_dir;
}

