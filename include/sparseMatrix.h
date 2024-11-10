#pragma once

#include <TArray.h>
#include <assert.h>
#include <iostream>
#include <stdexcept>

struct Cooef
{
    int i;
    int j;
    double val;
};

class SparseMatrix
{
public:
    int rows;
    int cols;
    int nnz; //  非零元素的数量
    struct Cooef *cooefs;

    SparseMatrix() : rows(0), cols(0), nnz(0), cooefs(nullptr) {}

    SparseMatrix(int r, int c) : rows(r), cols(c), nnz(0), cooefs(nullptr) {}

    SparseMatrix(int r, int c, int nonZeroElements)
        : rows(r), cols(c), nnz(nonZeroElements), cooefs(new Cooef[nonZeroElements]) {}

    ~SparseMatrix() { delete[] cooefs; }

    // 禁用拷贝构造和赋值运算符
    SparseMatrix(const SparseMatrix &) = delete;
    SparseMatrix &operator=(const SparseMatrix &) = delete;

    void print();
};

Vec SMVP(const SparseMatrix &M, const Vec &v)
{
    Vec out(M.rows, 0);

    if (M.cols != v.size)
    {
        throw std::invalid_argument("Size mismatch: The number of columns in the matrix does not match the size of the vector.");
    }

    // 稀疏矩阵-向量乘法
    for (int idx = 0; idx < M.nnz; ++idx)
    {
        struct Cooef *tmp = &M.cooefs[idx];
        out[tmp->i] += tmp->val * v[tmp->j];
    }
    return out;
}

void SparseMatrix::print()
{
    for (int i = 0; i < nnz; ++i)
    {
        std::cout << "i: " << cooefs[i].i << " j: " << cooefs[i].j << " val: " << cooefs[i].val << std::endl;
    }
}