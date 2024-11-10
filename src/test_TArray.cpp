#include <TArray.h>
#include <iostream>

int main()
{
    int size = 5;
    TArray<int> arr(size, 1);
    std::cout << "arr: " << arr << std::endl;
    for (int i = 0; i < size; ++i)
    {
        arr[i] += 1;
    }

    std::cout << "arr: " << arr << std::endl;

    TArray<int> arr2 = arr;
    std::cout << "arr2: " << arr2 << std::endl;

    TArray<int> arr3, arr4;
    arr3 = arr4 = arr2;

    std::cout << "arr3: " << arr3 << std::endl;
    std::cout << "arr4: " << arr4 << std::endl;

    arr.push_back(100);
    std::cout << "arr: " << arr << std::endl;
    std::cout << "capacity of arr : " << arr.get_capacity() << std::endl;

    for (int i = 0; i < 10; ++i)
    {
        arr.push_back(i);
    }
    std::cout << "arr: " << arr << std::endl;
    std::cout << "capacity of arr : " << arr.get_capacity() << std::endl;

    arr4 = arr;
    std::cout << "arr4: " << arr4 << std::endl;
    std::cout << "capacity of arr4 : " << arr4.get_capacity() << std::endl;   

    std::cout << "----测试向量运算----" << std::endl;

    TArray<float> a(10,1.8);
    TArray<float> b(10,2);
    TArray<float> c(8,1);
    float n = 30;

    std::cout << "a * n = " << a * n << std::endl;
    std::cout << "a / n = " << a / n << std::endl;
    std::cout << "a + b = " << a + b << std::endl;
    std::cout << "a - b = " << a - b << std::endl;
    std::cout << "-a = " << -a << std::endl;

    a -= b;
    std::cout << "a -= b " << a << std::endl;

    a -= b;
    std::cout << "a -= b " << a << std::endl;

    std::cout << "a dot b " << dot(a,b) << std::endl;
    std::cout << "norm of c " << c.norm() << std::endl;

    return 0;
}