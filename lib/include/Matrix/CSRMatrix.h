#pragma once

#include <Mesh.h>
#include <Matrix.h>
#include <TArray.h>

class CSRMatrix : public Matrix
// 按行存储的稀疏矩阵, 存储每行不为零的元素
{
public:
    Vec elements;
    TArray<size_t> row_offset;
    TArray<size_t> elm_idx;
    Mesh &mesh;

    CSRMatrix(Mesh &m); // 根据Mesh中每个顶点之间的连通性建立
    ~CSRMatrix() = default;

    void MVP(const Vec &x, Vec &y) const;
};

void blas_addMatrix(const CSRMatrix &M, double val, const CSRMatrix &S, CSRMatrix &A);
// 计算A = S + val * M
