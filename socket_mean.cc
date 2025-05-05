#include <iostream>     // 用于 std::cout 和 std::cerr
#include <cstring>      // 用于 std::memset
#include <unistd.h>     // 用于 close
#include <sys/types.h>  // 用于 ssize_t
#include <sys/socket.h> // 用于 socket 和 recvfrom
#include <netinet/in.h> // 用于 struct sockaddr_in
#include <arpa/inet.h>  // 用于 inet_ntoa 和 htons

void recvfrom()
{
// 返回值：
// >= 0	实际接收到的字节数（即数据长度）
// == 0	对于 UDP 来说通常不会返回 0，但在某些系统中表示收到一个空报文
// < 0	出错，返回 -1，并设置 errno 指示错误类型
ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags,
    struct sockaddr *src_addr, socklen_t *addrlen);
// buf是用户的缓冲区，用来存放发送来的sockfd中的内容；
// len是buf的大小（长度)，表示接收的最大字节数；
// flags：控制接收行为的标志位（如是否非阻塞、是否仅查看数据）；
// 阻塞式IO：直到对方发送数据，才进行下一步，否则一直阻塞在此；
// 如果套接字是阻塞的，recvfrom 会阻塞直到接收到数据或出错；
// 如果是非阻塞的，recvfrom 会立即返回，没有数据时返回 -1 并设置 errno。
// src_addr：发送方地址结构体；
// addrlen：src_addr 结构体长度，调用后会被更新为实际长度

// flags的类型:
// 0：默认行为，使用套接字的阻塞 / 非阻塞属性，不做额外干预。
// MSG_PEEK：查看数据但不移除缓冲区内容。
// MSG_DONTWAIT：让本次调用非阻塞。
// MSG_WAITALL：等到接收到全部指定字节才返回。
}

void sendto()
{

ssize_t sendto(int sockfd, const void *buf, size_t len, int flags,
    const struct sockaddr *dest_addr, socklen_t addrlen);
// sockfd：套接字文件描述符，UDP 类型（SOCK_DGRAM）
// buf：要发送的数据指针，转成 C 风格字符串
// len：实际要发送的字节数（不包括末尾的 \0）
// flags：默认行为，通常设为 0
// dest_addr：目标地址（接收方）
// addrlen：目标地址结构体的长度
    
}
