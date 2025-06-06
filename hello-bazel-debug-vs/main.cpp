#include <iostream>
#include <vector>

int calculateSum(const std::vector<int>& numbers) {
    int sum = 0;
    // 这里可以设置断点来观察循环过程
    for (int num : numbers) {
        sum += num;
    }
    return sum;
}

int main() {
    std::vector<int> numbers = {1, 2, 3, 4, 5};
    
    std::cout << "Debug example program\n";
    std::cout << "Numbers: ";
    for (int num : numbers) {
        std::cout << num << " ";
    }
    std::cout << "\n";

    // 这里可以设置断点来查看计算结果
    int sum = calculateSum(numbers);
    std::cout << "Sum: " << sum << "\n";

    return 0;
}