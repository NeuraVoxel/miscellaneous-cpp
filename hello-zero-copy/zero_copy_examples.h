#pragma once

#include <string>
#include <chrono>
#include <functional>

namespace zero_copy {

// 传统的文件复制方法（使用read/write系统调用）
bool traditional_copy(const std::string& src_path, const std::string& dst_path, size_t buffer_size = 4096);

// 使用mmap/munmap的零拷贝方法
bool mmap_copy(const std::string& src_path, const std::string& dst_path);

// 使用sendfile的零拷贝方法
bool sendfile_copy(const std::string& src_path, const std::string& dst_path);

// 使用splice的零拷贝方法
bool splice_copy(const std::string& src_path, const std::string& dst_path);

// 测量函数执行时间的辅助函数
template<typename Func, typename... Args>
std::chrono::microseconds measure_time(Func&& func, Args&&... args) {
    auto start = std::chrono::high_resolution_clock::now();
    std::forward<Func>(func)(std::forward<Args>(args)...);
    auto end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::microseconds>(end - start);
}

// 比较不同复制方法的性能
void compare_copy_methods(const std::string& src_path, const std::string& dst_path);

} // namespace zero_copy