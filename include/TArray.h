#pragma once

#include <sys_utils.h>
#include <stddef.h>
#include <iostream>
#include <algorithm>

template <typename T>
class TArray
{
public:
    // 构造函数
    TArray();                       // 无参数的构造函数，生成一个空的TArray
    TArray(size_t);                 // 生成指定大小的TArray
    TArray(size_t, T);              // 生成指定大小且内部元素初始化为指定值的TArray
    TArray(const TArray<T> &other); // 拷贝构造函数,即使用另一个现有的TArray来构造新的TArray

    // 析构函数，用于在对象生命周期结束时执行清理操作，这里要将data的地址释放
    ~TArray();

    // 运算符重载
    T &operator[](size_t i); // 重载[]运算符
    const T &operator[](size_t i) const;

    TArray &operator=(const TArray<T> &other); // 重载=赋值运算符

    template <typename U>
    friend std::ostream &operator<<(std::ostream &os, const TArray<U> &arr); // 通过声明友元的方式重载<<以使用cout输出

    // 方法
    size_t get_size() const;     // 获取TArray的元素个数
    size_t get_capacity() const; // 获取容量
    void push_back(const T &t);  // 向末尾增加一个元素,值为t
    void resize(size_t size);    // 修改Array的size
    T *erase(T *pos);
    T *erase(T *first, T *last);

    // begin() 与 end() 以支持迭代器
    T *begin() { return data; }
    T *end() { return data + size; }
    const T *begin() const { return data; }
    const T *end() const { return data + size; }



    // TArray中的成员
    size_t size;     // 当前array的元素个数
    size_t capacity; // 当前分配的容量
    T *data;         // 存储的T数组
};

// 构造函数
template <typename T>
TArray<T>::TArray()
    : size{0}, capacity{0}, data{nullptr}
{
}

template <typename T>
TArray<T>::TArray(size_t s)
    : size{s}, capacity{s}
{
    data = new T[s];
}

template <typename T>
TArray<T>::TArray(size_t s, T val)
    : size{s}, capacity{s}
{
    data = new T[s];
    for (int i = 0; i < s; ++i)
    {
        data[i] = val;
    }
}

template <typename T>
TArray<T>::TArray(const TArray<T> &other)        // 使用另一个TArray<T>对象来初始化
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
TArray<T>::~TArray()
{
    delete[] data;
}

// 运算符重载
template <typename T>
inline T &TArray<T>::operator[](size_t i)
{
    return data[i];
}

template <typename T>
inline const T &TArray<T>::operator[](size_t i) const
{
    return data[i];
}

template <typename T>
inline TArray<T> &TArray<T>::operator=(const TArray<T> &other)
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
std::ostream &operator<<(std::ostream &os, const TArray<T> &arr)
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
inline size_t TArray<T>::get_size() const { return size; }

template <typename T>
inline size_t TArray<T>::get_capacity() const { return capacity; }

template <typename T>
void TArray<T>::push_back(const T &t)
{
    // 首先检查容量是否足够
    if (size >= capacity)
    {
        capacity = capacity ? 2 * capacity : 1; // 三元运算符，若capacity为true(即不为0)，则将容量翻倍；若为false(即初始为0)，则设置为1
        data = static_cast<T *>(safe_realloc(data, capacity * sizeof(T)));
    }
    data[size++] = t;
}

template <typename T>
void TArray<T>::resize(size_t s)
{
    // 首先检查容量是否足够
    if (s > capacity)
    {
        data = static_cast<T *>(safe_realloc(data, s * sizeof(T)));
        capacity = s;
    }
    size = s;
}

// 删除单个元素
template <typename T>
T *TArray<T>::erase(T *pos)
{
    if (pos >= data && pos < data + size)
    {
        // 将 pos 之后的元素前移一个位置
        std::move(pos + 1, data + size, pos);
        --size;
    }
    return pos;
}

// 删除范围内的元素
template <typename T>
T *TArray<T>::erase(T *first, T *last)
{
    if (first >= data && last <= data + size && first <= last)
    {
        // 将 last 之后的元素前移
        auto new_end = std::move(last, data + size, first);
        size -= (last - first); // 更新 size
    }
    return first;
}