#pragma once

#include <sys_utils.h>
#include <stddef.h>
#include <iostream>
#include  <algorithm>

template <typename T>
class Tarray
{
public:
    // 构造函数
    Tarray();                       // 无参数的构造函数，生成一个空的Tarray
    Tarray(size_t);                 // 生成指定大小的Tarray
    Tarray(size_t, T);              // 生成指定大小且内部元素初始化为指定值的Tarray
    Tarray(const Tarray<T> &other); // 拷贝构造函数,即使用另一个现有的Tarray来构造新的Tarray

    // 析构函数，用于在对象生命周期结束时执行清理操作，这里要将data的地址释放
    ~Tarray();

    // 运算符重载
    T &operator[](size_t i); // 重载[]运算符
    const T &operator[](size_t i) const;

    Tarray &operator=(const Tarray<T> &other); // 重载=赋值运算符

    template <typename U>
    friend std::ostream &operator<<(std::ostream &os, const Tarray<U> &arr); // 通过声明友元的方式重载<<以使用cout输出

    // 方法
    size_t get_size() const;     // 获取Tarray的元素个数
    size_t get_capacity() const; // 获取容量
    void push_back(const T &t);  // 向末尾增加一个元素,值为t

    

private:
    // Tarray中的成员
    size_t size;     // 当前array的元素个数
    size_t capacity; // 当前分配的容量
    T *data;         // 存储的T数组
};

// 构造函数
template <typename T>
Tarray<T>::Tarray()
    : size{0}, capacity{0}, data{nullptr}
{
}

template <typename T>
Tarray<T>::Tarray(size_t s)
    : size{s}, capacity{s}
{
    data = new T[s];
}

template <typename T>
Tarray<T>::Tarray(size_t s, T val)
    : size{s}, capacity{s}
{
    data = new T[s];
    for (int i = 0; i < s; ++i)
    {
        data[i] = val;
    }
}

template <typename T>
Tarray<T>::Tarray(const Tarray<T> &other)        // 使用另一个Tarray<T>对象来初始化
    : size{other.size}, capacity{other.capacity} // 同一个类的对象可以访问另一个对象的private参数，private和public是对于不同类而言的
{
    if (!other.data)
    {
        data = nullptr;
    }
    else
    {
        data = new T[size];
        std::copy(other.data, other.data + size, data);
    }
}

// 析构函数
template <typename T>
Tarray<T>::~Tarray()
{
    delete[] data;
}

// 运算符重载
template <typename T>
inline T &Tarray<T>::operator[](size_t i)
{
    return data[i];
}

template <typename T>
inline const T &Tarray<T>::operator[](size_t i) const
{
    return data[i];
}

template <typename T>
inline Tarray<T> &Tarray<T>::operator=(const Tarray<T> &other)
{
    // this 是指向当前对象的指针
    if (this == &other)
    {
        return *this;
    }

    delete[] data;

    size = other.size;
    capacity = other.capacity;
    if (!other.data)
    {
        data = nullptr;
    }
    else
    {
        data = new T[size];
        std::copy(other.data, other.data + size, data);
    }

    return *this; // 返回当前对象的引用，以支持连续赋值
}

template <typename T>
std::ostream &operator<<(std::ostream &os, const Tarray<T> &arr)
{
    os << "[";
    for (size_t i = 0; i < arr.size; ++i)
    {
        os << arr.data[i];
        if (i < arr.size - 1)
        {
            os << ", ";
        }
    }
    os << "]";
    return os;
}

// 成员函数
template <typename T>
inline size_t Tarray<T>::get_size() const { return size; }

template <typename T>
inline size_t Tarray<T>::get_capacity() const { return capacity; }

template <typename T>
void Tarray<T>::push_back(const T &t)
{
    // 首先检查容量是否足够
    if (size >= capacity)
    {
        capacity = capacity ? 2 * capacity : 1; // 三元运算符，若capacity为true(即不为0)，则将容量翻倍；若为false(即初始为0)，则设置为1
        data = static_cast<T*>(safe_realloc(data, capacity * sizeof(T)));
    }
    data[size++] = t;
}