#include <FTP_client.hpp>

int main()
{
    FTPClient Client(PORT);
    Client.init();
    Client.start();
    return 0;
}