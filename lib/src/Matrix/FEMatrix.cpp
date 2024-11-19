#include <FEMatrix.h>
#include <TArray.h>
#include <Mesh.h>
#include <cstdint>


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