#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>

#include "Server.hpp"

int main()
{
    Server serve(8080);
    serve.init();
    serve.start();

    return 0;
}