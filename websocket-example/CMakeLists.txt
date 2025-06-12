cmake_minimum_required(VERSION 3.10)
project(websocket_example)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 添加调试信息
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")

# 查找Boost库
find_package(Boost REQUIRED COMPONENTS system)

# 添加头文件路径
include_directories(${Boost_INCLUDE_DIRS})

# 创建服务器可执行文件
add_executable(websocket_server websocket_server.cpp)
target_link_libraries(websocket_server PRIVATE ${Boost_LIBRARIES})

# 创建客户端可执行文件
add_executable(websocket_client websocket_client.cpp)
target_link_libraries(websocket_client PRIVATE ${Boost_LIBRARIES})

# 设置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)