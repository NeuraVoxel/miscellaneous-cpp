#include <iostream>
#include <vector>
#include <string>

// 计算斐波那契数列的第n个数
int fibonacci(int n) {
    if (n <= 1) return n;
    return fibonacci(n - 1) + fibonacci(n - 2);
}

// 检查一个数是否为质数
bool isPrime(int number) {
    if (number <= 1) return false;
    for (int i = 2; i * i <= number; i++) {
        if (number % i == 0) return false;
    }
    return true;
}

// 处理用户输入的数字
void processNumber(int num) {
    std::cout << "\nProcessing number: " << num << std::endl;
    
    // 计算斐波那契数
    std::cout << "Fibonacci(" << num << ") = " << fibonacci(num) << std::endl;
    
    // 检查是否为质数
    if (isPrime(num)) {
        std::cout << num << " is a prime number" << std::endl;
    } else {
        std::cout << num << " is not a prime number" << std::endl;
    }
    
    // 显示所有因子
    std::cout << "Factors of " << num << ": ";
    for (int i = 1; i <= num; i++) {
        if (num % i == 0) {
            std::cout << i << " ";
        }
    }
    std::cout << std::endl;
}

int main() {
    std::cout << "Welcome to the Number Analyzer!" << std::endl;
    
    std::vector<int> numbers;
    std::string input;
    
    // 收集用户输入的数字
    while (true) {
        std::cout << "\nEnter a number (or 'q' to quit): ";
        std::getline(std::cin, input);
        
        if (input == "q" || input == "Q") {
            break;
        }
        
        try {
            int num = std::stoi(input);
            numbers.push_back(num);
            processNumber(num);
        } catch (const std::exception& e) {
            std::cout << "Invalid input. Please enter a valid number." << std::endl;
        }
    }
    
    // 显示输入历史
    if (!numbers.empty()) {
        std::cout << "\nYou analyzed the following numbers:" << std::endl;
        for (int num : numbers) {
            std::cout << num << " ";
        }
        std::cout << std::endl;
    }
    
    return 0;
}