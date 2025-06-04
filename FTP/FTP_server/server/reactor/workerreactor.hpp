#pragma once

#include "include.hpp"
#include "../connect/FTPconnection.hpp"
#include "../thread_pool/ThreadSafeQueue.hpp"


// #define NUM 8

class worker{
public:
    worker();
    ~worker();
    void start();
    void make_queue(int cli);
    void join();
private:
    int make_nonblocking(int cli);
    void event_loop();
    void hand_io(int cli);
    void clean_client(int cli);

    std::vector<int> cli_fd;
    std::thread _thread;
    std::mutex cli_mutex;
    std::map<int,std::shared_ptr<FTPconnect>> connections;
    threadsafequeue<int> _queue;
    int epoll_worker;
    int _time;
    bool _running;
};