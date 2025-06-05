#include <iostream>
#include <vector>

int factorial(int n)
{
    if (n <= 1)
        return 1;
    return n * factorial(n - 1);
}

int main()
{
    std::vector<int> numbers = {1, 2, 3, 4, 5};
    int sum = 0;

    // 这个循环可以用来演示调试
    for (int i = 0; i < numbers.size(); ++i)
    {
        sum += numbers[i];
        std::cout << "Current sum: " << sum << std::endl;
    }

    // 调用阶乘函数，可以用来演示函数调用栈
    int fact = factorial(5);
    std::cout << "Factorial of 5: " << fact << std::endl;

    return 0;
}