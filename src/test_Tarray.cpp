#include <Tarray.h>
#include <iostream>

int main()
{
    int size = 5;
    Tarray<int> arr(size, 1);
    std::cout << "arr: " << arr << std::endl;
    for (int i = 0; i < size; ++i)
    {
        arr[i] += 1;
    }

    std::cout << "arr: " << arr << std::endl;

    Tarray<int> arr2 = arr;
    std::cout << "arr2: " << arr2 << std::endl;

    Tarray<int> arr3, arr4;
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


    return 0;
}