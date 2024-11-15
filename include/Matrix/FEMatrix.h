#pragma once

#include <TArray.h>
#include <Matrix.h>
#include <Mesh.h>

class FEMatrix : public Matrix
// 专门为P1元生成矩阵使用
{
public:
    enum FEMType
    {
        P1_Mass,
        P1_Stiffness
    };
    Vec diag; // 存储对角线元素
    Vec offdiag;
    FEMType femtype;
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

    // FEMType表示矩阵的类型是质量还是刚度
    FEMatrix(Mesh &mesh, FEMType fem_type) : Matrix(mesh.vertex_count(), mesh.vertex_count()), diag(mesh.vertex_count(), 0.0), m(mesh), femtype(fem_type)
    {
        if (femtype == P1_Mass)
        {
            offdiag.resize(mesh.triangle_count());
        }
        else if (femtype == P1_Stiffness)
        {
            offdiag.resize(3 * mesh.triangle_count());
        }
        offdiag.setAll(0);
    }
    ~FEMatrix() = default;

    void MVP(const Vec &x, Vec &y) const;
};

void MVP_P1_Mass(const FEMatrix &M, const Vec &x, Vec &y)
{
    y.setAll(0);

    // 先计算对角线
    for (int i = 0; i < M.rows; ++i)
    {
        y[i] = M.diag[i] * x[i];
    }

    // 根据三角形依次计算非对角线
    for (size_t t = 0; t < M.m.triangle_count(); ++t)
    {
        uint32_t a = M.m.indices[3 * t];
        uint32_t b = M.m.indices[3 * t + 1];
        uint32_t c = M.m.indices[3 * t + 2];

        // 每个三角形仅对应offdiag中的一个元素，但
        double val = M.offdiag[t];

        y[a] += val * x[b];
        y[b] += val * x[a];
        y[a] += val * x[c];
        y[c] += val * x[a];
        y[b] += val * x[c];
        y[c] += val * x[b];
        // 同时还有下三角的部分
    }
}

void MVP_P1_Sniffness(const FEMatrix &M, const Vec &x, Vec &y)
{
    y.setAll(0);

    // 先计算对角线
    for (int i = 0; i < M.rows; ++i)
    {
        y[i] = M.diag[i] * x[i];
    }

    // 根据三角形依次计算非对角线
    for (size_t t = 0; t < M.m.triangle_count(); ++t)
    {
        uint32_t a = M.m.indices[3 * t];
        uint32_t b = M.m.indices[3 * t + 1];
        uint32_t c = M.m.indices[3 * t + 2];

        // 按照AB, AC, BC的顺序存储非对角线元素
        // 因此是a行b列，a行c列，b行c列的顺序
        y[a] += M.offdiag[3 * t + 0] * x[b];
        y[b] += M.offdiag[3 * t + 0] * x[a];
        y[a] += M.offdiag[3 * t + 1] * x[c];
        y[c] += M.offdiag[3 * t + 1] * x[a];
        y[b] += M.offdiag[3 * t + 2] * x[c];
        y[c] += M.offdiag[3 * t + 2] * x[b];
        // 同时还有下三角的部分
    }
}

void FEMatrix::MVP(const Vec &x, Vec &y) const
// 按照根据不同的FEMType计算
{
    switch (femtype)
    {
    case P1_Mass:
        MVP_P1_Mass(*this, x, y);
        break;
    case P1_Stiffness:
        MVP_P1_Sniffness(*this, x, y);
    default:
        break;
    }
}