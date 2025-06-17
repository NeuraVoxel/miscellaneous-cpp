#include "zero_copy_examples.h"
#include <iostream>
#include <string>
#include <unistd.h>

void print_usage(const char* program_name) {
    std::cout << "Usage: " << program_name << " <source_file> <destination_file>" << std::endl;
    std::cout << "\nThis program demonstrates and compares different file copy methods:" << std::endl;
    std::cout << "1. Traditional copy (using read/write system calls)" << std::endl;
    std::cout << "2. Zero-copy using mmap/munmap" << std::endl;
#if defined(__linux__) || defined(__APPLE__)
    std::cout << "3. Zero-copy using sendfile" << std::endl;
#endif
#ifdef __linux__
    std::cout << "4. Zero-copy using splice" << std::endl;
#endif
    std::cout << "\nThe program will create multiple copies of the source file with different extensions:" << std::endl;
    std::cout << "- .traditional: using traditional copy method" << std::endl;
    std::cout << "- .mmap: using mmap/munmap method" << std::endl;
#if defined(__linux__) || defined(__APPLE__)
    std::cout << "- .sendfile: using sendfile method" << std::endl;
#endif
#ifdef __linux__
    std::cout << "- .splice: using splice method" << std::endl;
#endif
}

bool file_exists(const std::string& path) {
    return access(path.c_str(), F_OK) != -1;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        print_usage(argv[0]);
        return 1;
    }

    std::string src_path = argv[1];
    std::string dst_path = argv[2];

    // 检查源文件是否存在
    if (!file_exists(src_path)) {
        std::cerr << "Error: Source file '" << src_path << "' does not exist." << std::endl;
        return 1;
    }

    // 检查目标文件是否已存在
    if (file_exists(dst_path)) {
        std::cout << "Warning: Destination file '" << dst_path << "' already exists and will be overwritten." << std::endl;
        std::cout << "Do you want to continue? (y/n): ";
        char response;
        std::cin >> response;
        if (response != 'y' && response != 'Y') {
            std::cout << "Operation cancelled by user." << std::endl;
            return 0;
        }
    }

    try {
        // 执行性能比较
        zero_copy::compare_copy_methods(src_path, dst_path);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    std::cout << "\nAll copy operations completed successfully." << std::endl;
    std::cout << "You can find the copied files with different extensions in the same directory as the destination file." << std::endl;
    
    return 0;
}