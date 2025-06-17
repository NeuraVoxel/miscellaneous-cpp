#include <iostream>
#include <fstream>
#include <random>
#include <string>
#include <algorithm>

// 创建一个指定大小的测试文件，填充随机数据
void create_random_file(const std::string& path, size_t size_mb) {
    std::cout << "Creating test file: " << path << " (" << size_mb << " MB)" << std::endl;
    
    // 打开输出文件流
    std::ofstream file(path, std::ios::binary);
    if (!file) {
        std::cerr << "Error: Could not open file for writing: " << path << std::endl;
        return;
    }
    
    // 设置随机数生成器
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(0, 255);
    
    // 计算总字节数
    size_t total_bytes = size_mb * 1024 * 1024;
    
    // 使用缓冲区提高写入效率
    const size_t buffer_size = 64 * 1024; // 64KB buffer
    char buffer[buffer_size];
    
    // 写入随机数据
    size_t bytes_written = 0;
    size_t percent_done = 0;
    
    while (bytes_written < total_bytes) {
        // 填充缓冲区
        for (size_t i = 0; i < buffer_size && bytes_written + i < total_bytes; ++i) {
            buffer[i] = static_cast<char>(distrib(gen));
        }
        
        // 计算本次写入的字节数
        size_t bytes_to_write = std::min(buffer_size, total_bytes - bytes_written);
        
        // 写入文件
        file.write(buffer, bytes_to_write);
        if (!file) {
            std::cerr << "Error: Failed to write to file: " << path << std::endl;
            file.close();
            return;
        }
        
        bytes_written += bytes_to_write;
        
        // 显示进度
        size_t new_percent = (bytes_written * 100) / total_bytes;
        if (new_percent > percent_done) {
            percent_done = new_percent;
            std::cout << "\rProgress: " << percent_done << "%" << std::flush;
        }
    }
    
    file.close();
    std::cout << "\nTest file created successfully: " << path << " (" << size_mb << " MB)" << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cout << "Usage: " << argv[0] << " <output_file_path> <size_in_MB>" << std::endl;
        std::cout << "Example: " << argv[0] << " test_file.bin 100" << std::endl;
        return 1;
    }
    
    std::string file_path = argv[1];
    
    // 解析文件大小参数
    size_t size_mb;
    try {
        size_mb = std::stoul(argv[2]);
        if (size_mb == 0) {
            throw std::invalid_argument("Size must be greater than 0");
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: Invalid size parameter. Please provide a positive integer." << std::endl;
        return 1;
    }
    
    create_random_file(file_path, size_mb);
    
    return 0;
}