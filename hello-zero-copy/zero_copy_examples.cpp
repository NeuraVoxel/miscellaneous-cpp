#include "zero_copy_examples.h"

#include <iostream>
#include <fstream>
#include <algorithm>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <string.h>
#include <errno.h>

#ifdef __linux__
#include <sys/sendfile.h>
#elif defined(__APPLE__)
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/uio.h>
#endif

namespace zero_copy {

// 获取文件大小的辅助函数
off_t get_file_size(int fd) {
    struct stat st;
    if (fstat(fd, &st) == -1) {
        std::cerr << "Error getting file size: " << strerror(errno) << std::endl;
        return -1;
    }
    return st.st_size;
}

// 传统的文件复制方法（使用read/write系统调用）
bool traditional_copy(const std::string& src_path, const std::string& dst_path, size_t buffer_size) {
    int src_fd = open(src_path.c_str(), O_RDONLY);
    if (src_fd == -1) {
        std::cerr << "Error opening source file: " << strerror(errno) << std::endl;
        return false;
    }

    int dst_fd = open(dst_path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (dst_fd == -1) {
        std::cerr << "Error opening destination file: " << strerror(errno) << std::endl;
        close(src_fd);
        return false;
    }

    char* buffer = new char[buffer_size];
    ssize_t bytes_read, bytes_written;
    bool success = true;

    while ((bytes_read = read(src_fd, buffer, buffer_size)) > 0) {
        char* ptr = buffer;
        while (bytes_read > 0) {
            bytes_written = write(dst_fd, ptr, bytes_read);
            if (bytes_written == -1) {
                std::cerr << "Error writing to destination file: " << strerror(errno) << std::endl;
                success = false;
                break;
            }
            bytes_read -= bytes_written;
            ptr += bytes_written;
        }
        if (!success) break;
    }

    if (bytes_read == -1) {
        std::cerr << "Error reading from source file: " << strerror(errno) << std::endl;
        success = false;
    }

    delete[] buffer;
    close(src_fd);
    close(dst_fd);
    return success;
}

// 使用mmap/munmap的零拷贝方法
bool mmap_copy(const std::string& src_path, const std::string& dst_path) {
    int src_fd = open(src_path.c_str(), O_RDONLY);
    if (src_fd == -1) {
        std::cerr << "Error opening source file: " << strerror(errno) << std::endl;
        return false;
    }

    off_t src_size = get_file_size(src_fd);
    if (src_size == -1) {
        close(src_fd);
        return false;
    }

    int dst_fd = open(dst_path.c_str(), O_RDWR | O_CREAT | O_TRUNC, 0644);
    if (dst_fd == -1) {
        std::cerr << "Error opening destination file: " << strerror(errno) << std::endl;
        close(src_fd);
        return false;
    }

    // 设置目标文件大小
    if (ftruncate(dst_fd, src_size) == -1) {
        std::cerr << "Error setting destination file size: " << strerror(errno) << std::endl;
        close(src_fd);
        close(dst_fd);
        return false;
    }

    // 映射源文件到内存
    void* src_mmap = mmap(NULL, src_size, PROT_READ, MAP_PRIVATE, src_fd, 0);
    if (src_mmap == MAP_FAILED) {
        std::cerr << "Error mapping source file: " << strerror(errno) << std::endl;
        close(src_fd);
        close(dst_fd);
        return false;
    }

    // 映射目标文件到内存
    void* dst_mmap = mmap(NULL, src_size, PROT_READ | PROT_WRITE, MAP_SHARED, dst_fd, 0);
    if (dst_mmap == MAP_FAILED) {
        std::cerr << "Error mapping destination file: " << strerror(errno) << std::endl;
        munmap(src_mmap, src_size);
        close(src_fd);
        close(dst_fd);
        return false;
    }

    // 直接在内存中复制数据
    memcpy(dst_mmap, src_mmap, src_size);

    // 解除映射
    munmap(src_mmap, src_size);
    munmap(dst_mmap, src_size);

    close(src_fd);
    close(dst_fd);
    return true;
}

// 使用sendfile的零拷贝方法
bool sendfile_copy(const std::string& src_path, const std::string& dst_path) {
#if defined(__linux__) || defined(__APPLE__)
    int src_fd = open(src_path.c_str(), O_RDONLY);
    if (src_fd == -1) {
        std::cerr << "Error opening source file: " << strerror(errno) << std::endl;
        return false;
    }

    int dst_fd = open(dst_path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (dst_fd == -1) {
        std::cerr << "Error opening destination file: " << strerror(errno) << std::endl;
        close(src_fd);
        return false;
    }

    off_t src_size = get_file_size(src_fd);
    if (src_size == -1) {
        close(src_fd);
        close(dst_fd);
        return false;
    }

    #ifdef __linux__
    // Linux implementation
    off_t offset = 0;
    while (offset < src_size) {
        ssize_t bytes_sent = sendfile(dst_fd, src_fd, &offset, src_size - offset);
        if (bytes_sent <= 0) {
            if (errno == EAGAIN || errno == EINTR) {
                // 可重试的错误，继续尝试
                continue;
            }
            std::cerr << "Error during sendfile: " << strerror(errno) << std::endl;
            close(src_fd);
            close(dst_fd);
            return false;
        }
    }
    #elif defined(__APPLE__)
    // macOS implementation
    // 在macOS上，sendfile的参数顺序与Linux不同
    // macOS: sendfile(源fd, 目标fd, 偏移量, 长度指针, 头尾结构体, 标志)
    off_t len = src_size;
    off_t offset = 0;
    while (offset < src_size) {
        // macOS sendfile返回0表示成功，-1表示错误
        // len会被更新为已发送的字节数
        int result = sendfile(src_fd, dst_fd, offset, &len, nullptr, 0);
        if (result == -1) {
            if (errno == EAGAIN || errno == EINTR) {
                // 可重试的错误，继续尝试
                // 在macOS上，len会被更新为已发送的字节数
                offset += len;
                len = src_size - offset;
                continue;
            }
            std::cerr << "Error during sendfile: " << strerror(errno) << std::endl;
            close(src_fd);
            close(dst_fd);
            return false;
        }
        // 成功发送数据
        offset += len;
        len = src_size - offset;
    }
    #endif

    close(src_fd);
    close(dst_fd);
    return true;
#else
    std::cerr << "sendfile is not available on this system" << std::endl;
    return false;
#endif
}

// 使用splice的零拷贝方法
bool splice_copy(const std::string& src_path, const std::string& dst_path) {
#ifdef __linux__
    int src_fd = open(src_path.c_str(), O_RDONLY);
    if (src_fd == -1) {
        std::cerr << "Error opening source file: " << strerror(errno) << std::endl;
        return false;
    }

    int dst_fd = open(dst_path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (dst_fd == -1) {
        std::cerr << "Error opening destination file: " << strerror(errno) << std::endl;
        close(src_fd);
        return false;
    }

    off_t src_size = get_file_size(src_fd);
    if (src_size == -1) {
        close(src_fd);
        close(dst_fd);
        return false;
    }

    int pipe_fds[2];
    if (pipe(pipe_fds) == -1) {
        std::cerr << "Error creating pipe: " << strerror(errno) << std::endl;
        close(src_fd);
        close(dst_fd);
        return false;
    }

    size_t bytes_remaining = src_size;
    const size_t chunk_size = 65536; // 64KB chunks
    
    while (bytes_remaining > 0) {
        size_t to_copy = std::min(bytes_remaining, chunk_size);
        
        // 从源文件拷贝到管道
        ssize_t bytes_spliced = splice(src_fd, NULL, pipe_fds[1], NULL, to_copy, SPLICE_F_MOVE);
        if (bytes_spliced <= 0) {
            if (errno == EAGAIN || errno == EINTR) {
                continue;
            }
            std::cerr << "Error splicing from source to pipe: " << strerror(errno) << std::endl;
            close(pipe_fds[0]);
            close(pipe_fds[1]);
            close(src_fd);
            close(dst_fd);
            return false;
        }
        
        // 从管道拷贝到目标文件
        ssize_t bytes_written = 0;
        while (bytes_written < bytes_spliced) {
            ssize_t res = splice(pipe_fds[0], NULL, dst_fd, NULL, bytes_spliced - bytes_written, SPLICE_F_MOVE);
            if (res <= 0) {
                if (errno == EAGAIN || errno == EINTR) {
                    continue;
                }
                std::cerr << "Error splicing from pipe to destination: " << strerror(errno) << std::endl;
                close(pipe_fds[0]);
                close(pipe_fds[1]);
                close(src_fd);
                close(dst_fd);
                return false;
            }
            bytes_written += res;
        }
        
        bytes_remaining -= bytes_spliced;
    }
    
    close(pipe_fds[0]);
    close(pipe_fds[1]);
    close(src_fd);
    close(dst_fd);
    return true;
#else
    std::cerr << "splice is only available on Linux systems" << std::endl;
    return false;
#endif
}

// 比较不同复制方法的性能
void compare_copy_methods(const std::string& src_path, const std::string& dst_path) {
    std::cout << "Comparing file copy methods for " << src_path << " -> " << dst_path << std::endl;
    
    // 获取源文件大小
    int src_fd = open(src_path.c_str(), O_RDONLY);
    if (src_fd == -1) {
        std::cerr << "Error opening source file: " << strerror(errno) << std::endl;
        return;
    }
    off_t file_size = get_file_size(src_fd);
    close(src_fd);
    
    if (file_size == -1) {
        return;
    }
    
    std::cout << "File size: " << file_size << " bytes" << std::endl;
    
    // 测试传统复制方法
    std::string traditional_dst = dst_path + ".traditional";
    auto traditional_time = measure_time(traditional_copy, src_path, traditional_dst, 4096);
    std::cout << "Traditional copy: " << traditional_time.count() << " microseconds" << std::endl;
    
    // 测试mmap复制方法
    std::string mmap_dst = dst_path + ".mmap";
    auto mmap_time = measure_time(mmap_copy, src_path, mmap_dst);
    std::cout << "mmap/munmap copy: " << mmap_time.count() << " microseconds" << std::endl;
    
#if defined(__linux__) || defined(__APPLE__)
    // 测试sendfile复制方法
    std::string sendfile_dst = dst_path + ".sendfile";
    auto sendfile_time = measure_time(sendfile_copy, src_path, sendfile_dst);
    std::cout << "sendfile copy: " << sendfile_time.count() << " microseconds" << std::endl;
#endif

#ifdef __linux__
    // 测试splice复制方法
    std::string splice_dst = dst_path + ".splice";
    auto splice_time = measure_time(splice_copy, src_path, splice_dst);
    std::cout << "splice copy: " << splice_time.count() << " microseconds" << std::endl;
#endif

#if !defined(__linux__) && !defined(__APPLE__)
    std::cout << "sendfile and splice methods are not available on this system" << std::endl;
#elif !defined(__linux__)
    std::cout << "splice method is only available on Linux systems" << std::endl;
#endif
    
    std::cout << "\nPerformance comparison (lower is better):" << std::endl;
    std::cout << "Traditional: 100%" << std::endl;
    std::cout << "mmap/munmap: " << (mmap_time.count() * 100.0 / traditional_time.count()) << "%" << std::endl;
    
#if defined(__linux__) || defined(__APPLE__)
    std::cout << "sendfile: " << (sendfile_time.count() * 100.0 / traditional_time.count()) << "%" << std::endl;
#endif

#ifdef __linux__
    std::cout << "splice: " << (splice_time.count() * 100.0 / traditional_time.count()) << "%" << std::endl;
#endif
}

} // namespace zero_copy