#include <cholesky.h>
#include <TArray.h>
#include <CSRMatrix.h>
#include <cmath>

static double computeDiag(CSRMatrix &A, CSRMatrix &L, int col);
static void computeNonDiag(CSRMatrix &A, CSRMatrix &L, int col, Vec &tmp);

/* Compute the Cholesky decomposition of a CSR matrix A
 * A = L * L^T
 * Use Skyling Format to store the matrix L
 */
Cholesky::Cholesky(CSRMatrix &A)
    : L(A)
{
    int n = A.rows;

    // Compute the first column
    int len = A.row_offset[1];
    L.row_offset[0] = 0;
    L.row_offset[1] = len;

    L.elm_idx.push_back(0); // 第一列第一个元素必然下标是0
    L.elements.push_back(std::sqrt(A.elements[0]));

    for (int k = 1; k < len; ++k)
    {
        L.elm_idx.push_back(A.elm_idx[k]);
        L.elements.push_back(A.elements[k] / L.elements[0]);
    }

    Vec tmp(n, 0.0); // Scratch array

    // 处理剩余的列
    for (int col = 1; col < n; ++col)
    {
        tmp[col] = computeDiag(A, L, col);
        computeNonDiag(A, L, col, tmp);

        // 将tmp压缩到L中
        int nnz = 0;
        for (int idx = col; idx < n; ++idx)
        {
            if (UNLIKELY(tmp[idx] != 0))
            {
                ++nnz;
                L.elements.push_back(tmp[idx]);
                L.elm_idx.push_back(idx);
            }
        }
        L.row_offset[col + 1] = L.row_offset[col] + nnz;
        tmp.setAll(0.0);
    }
}

// 求解L*L^Tx = b
void Cholesky::solve(Vec &b, Vec &x)
{
    // 先求解L y = b，同样注意这里的L是转置后的
    Vec y(x.size);
    y[0] = b[0] / L.elements[0];

    for (int i = 1; i < x.size; ++i)
    {
        double sum = 0;
        for (int k = 0; k < i; ++k)
        {
            sum += L(k, i) * y[k];
        }
        y[i] = (b[i] - sum) / L(i, i);
    }

    // 求解L^T x = y
    int n = x.size;
    x[n - 1] = y[n - 1] / L.elements[L.elements.size - 1];

    for (int i = n - 2; i >= 0; --i)
    {
        double sum = 0;
        for (int k = n - 1; k > i; --k)
        {
            sum += L(i, k) * x[k];
        }
        x[i] = (y[i] - sum) / L(i, i);
    }
}

// 计算col列的对角元素, 需要注意，这里L存储的是转置后的结果，因此取元素时需要进行转置
static double computeDiag(CSRMatrix &A, CSRMatrix &L, int col)
{
    double sum = 0;
    int start = 0;
    for (int k = 0; k < col; ++k)
    {
        double num = L.elements[start + col - k];
        sum += std::pow(num, 2);
        start = L.row_offset[k + 1];
    }
    return std::sqrt(A(col, col) - sum);
}

/* 通过L L^T = A 计算第col列时，可以将右端L^T的第col列缓存，加速计算
 */
static void computeNonDiag(CSRMatrix &A, CSRMatrix &L, int col, Vec &tmp)
{
    int n = A.rows;
    Vec rightCol(col + 1);
    // 先进行缓存
    int rightStart = 0;
    for (int k = 0; k <= col; ++k)
    {
        rightCol[k] = L.elements[rightStart + col - k];
        rightStart = L.row_offset[k + 1];
    }

    for (int i = col + 1; i < n; ++i)
    {
        double sum = 0;
        int leftStart = 0;
        for (int k = 0; k < col; ++k)
        {
            sum += L.elements[leftStart + i - k] * rightCol[k];
            leftStart = L.row_offset[k + 1];
        }
        tmp[i] = (A(i, col) - sum) / tmp[col]; 
    }
}