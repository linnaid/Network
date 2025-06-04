cmake_minimum_required(VERSION 3.10)

# 项目名称和版本
project(FTP VERSION 1.0)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 包含头文件目录（你写的和 json.hpp）
include_directories(include external)
include_directories(include Client)
include_directories(include FTP_server)
include_directories(include FTP_server/server/connect)
include_directories(include FTP_server/server/command)
include_directories(include FTP_server/server/reactor)
include_directories(include FTP_server/server/thread_pool)

# 设置输出目录
# set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build)

# 定义源码文件
set(SERVER_SRC
    FTP_server/server/reactor/master.cc
    FTP_server/main.cc
    FTP_server/server/reactor/worker.cc
    FTP_server/server/connect/FTPconnect.cc
    FTP_server/server/connect/Dataconnect.cc
    FTP_server/server/command/FTPcommand.cc
    FTP_server/server/thread_pool/ThreadSafeQueue.cc
)

set(CLIENT_SRC
    Client/main.cc
    Client/FTP_client.cc
    # client/data_client.cc
)

# 添加可执行文件
add_executable(server ${SERVER_SRC})
add_executable(client ${CLIENT_SRC})


# 添加链接库
# 这里可以根据需要添加线程库（例如 pthreads）：
target_link_libraries(server pthread)
target_link_libraries(client pthread)



