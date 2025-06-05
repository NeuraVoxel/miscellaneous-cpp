#include <iostream>
#include <vector>

int calculateSum(const std::vector<int>& numbers) {
    int sum = 0;
    for (int num : numbers) {
        // 这里是一个好的断点位置
        sum += num;
    }
    return sum;
}

int main() {
    std::vector<int> numbers = {1, 2, 3, 4, 5};
    
    // 这里也是一个好的断点位置
    int sum = calculateSum(numbers);
    
    std::cout << "Sum of numbers: " << sum << std::endl;
    
    // 添加一些交互式输入，方便调试
    int userInput;
    std::cout << "Enter a number to add to the sum: ";
    std::cin >> userInput;
    
    // 另一个好的断点位置
    sum += userInput;
    
    std::cout << "New sum: " << sum << std::endl;
    
    return 0;
}