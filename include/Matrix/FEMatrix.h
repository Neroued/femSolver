#pragma once

#include <TArray.h>
#include <Matrix.h>
#include <Mesh.h>

class FEMatrix : public Matrix
// 专门为P1元生成矩阵使用
{
public:
    Vec diag; // 存储对角线元素
    Vec offdiag;
    /* offdiag存储非对角线元素
     * 对于每一个三角形，都可以构建出一个3x3的loacl质量或刚度矩阵
     * 在质量矩阵M_loc中，只有两个不同的值：对角线元素均为一个值，非对角线元素均为一个值，
     * 因此可以offdiag和diag的size是相同的
     * 对于刚度矩阵，这是一个对称矩阵，因此只需要存储上三角中的元素，即6个元素
     * diag存储3个，offdiag存储三个
     * 因此若有n个三角形
     * M.diag有n个元素, M.offdiag有n个元素
     * S.diag有n个元素，S.offdiag有3n个元素
     * 可以使用一个函数将M添加到S中方便计算
     * 同时offdiag中元素的实际位置需要与Mesh进行对应，因此需要引入对应的Mesh
     */
    Mesh &m;

    // r 表示一个三角形对应几个offdiag中元素, 对于质量矩阵是1，刚度矩阵是3
    FEMatrix(Mesh &mesh, int r) : Matrix(mesh.vertex_count(), mesh.vertex_count()), diag(mesh.vertex_count(), 0.0), m(mesh)
    {   
        offdiag.resize(r * mesh.triangle_count());
        offdiag.setAll(0);
    }
    ~FEMatrix() = default;

    void MVP(const Vec &x, Vec &y) const;
};

void FEMatrix::MVP(const Vec &x, Vec &y) const
// 按照刚度矩阵的特点进行计算，质量矩阵可以被加到刚度矩阵中
{
    y.setAll(0);

    // 先计算对角线
    for (int i = 0; i < rows; ++i)
    {
        y[i] = diag[i] * x[i];
    }

    // 根据三角形依次计算非对角线
    for (size_t t = 0; t < m.triangle_count(); ++t)
    {
        uint32_t a = m.indices[3 * t];
        uint32_t b = m.indices[3 * t + 1];
        uint32_t c = m.indices[3 * t + 2];

        // 按照AB, AC, BC的顺序存储非对角线元素
        // 因此是a行b列，a行c列，b行c列的顺序
        y[a] += offdiag[3 * t + 0] * x[b];
        y[b] += offdiag[3 * t + 0] * x[a];
        y[a] += offdiag[3 * t + 1] * x[c];
        y[c] += offdiag[3 * t + 1] * x[a];
        y[b] += offdiag[3 * t + 2] * x[c];
        y[c] += offdiag[3 * t + 2] * x[b];
        // 同时还有下三角的部分
    }
}