//soceket编程接口

#include <iostream>
#include <unistd.h>

//创建socket文件描述符(TCP/UDP, 客户端+服务器)
int socket(int domain, int type, int protocol);

//绑定端口号(TCP/UDP, 服务器)
int bind(int socket, const struct sockaddr* address, socklen_t address_len);

//开始监听socket(TCP, 服务器)
int listen(int socket, int backlog);

//接收请求(TCP, 服务器)
int accept(int socket, struct socketaddr* address, socklen_t* address_len);

//建立连接(TCP, 客户端)
int connect(int sockfd, const struct sockaddr* addr, socklen_t addrlen);

//sockaddr结构
int accept(int socket, struct socketaddr* address, socklen_t* address_len);
//struct socketaddr* address会自动区分本地通信还是unix域间通信；
//AF_INET是本地通信；
//AF_UNIX是unix域间通信(也就是网络通信)；