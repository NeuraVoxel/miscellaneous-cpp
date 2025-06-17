cmake_minimum_required(VERSION 3.10)
project(hello-zero-copy CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 添加可执行文件
add_executable(zero_copy_demo
    main.cpp
    zero_copy_examples.cpp
)

# 添加测试文件生成器
add_executable(create_test_file
    create_test_file.cpp
)

# 在Linux系统上链接必要的库
if(UNIX AND NOT APPLE)
    target_link_libraries(zero_copy_demo PRIVATE pthread)
endif()

# 设置编译选项
target_compile_options(zero_copy_demo PRIVATE
    $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:Clang>:-Wall -Wextra -Wpedantic>
)

target_compile_options(create_test_file PRIVATE
    $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:Clang>:-Wall -Wextra -Wpedantic>
)