#include <cholesky.h>
#include <TArray.h>
#include <CSRMatrix.h>
#include <cmath>

/* Compute the Cholesky decomposition of a CSR matrix A
 * A = L * L^T
 * Use Skyline Format to store the matrix L
 */
Cholesky::Cholesky(CSRMatrix &A_CSR)
    : L(A_CSR)
{
    // 将A转换成SKR格式方便计算
    SKRMatrix A(A_CSR);
    A.convertFromCSR(A_CSR);

    // 计算每一行开始元素的下标
    TArray<int> minElmIdx(L.rows);
    minElmIdx[0] = 0;
    for (int row = 1; row < L.rows; ++row)
    {
        int SKR_start = L.column_offset[row];
        int SKR_len = L.column_offset[row + 1] - SKR_start;
        minElmIdx[row] = row - SKR_len + 1;
    }

    // 先计算第一列
    L.elements[0] = std::sqrt(A.elements[0]);
#pragma omp parallel for
    for (int row = 1; row < A.rows; ++row)
    {
        if (LIKELY(0 < minElmIdx[row])) // 假如第一列的第row行的元素为零直接跳过
        {
            continue;
        }
        else
        {
            L.elements[L.column_offset[row]] = A.elements[L.column_offset[row]] / L.elements[0];
        }
    }

    // 处理剩下的列
    for (int col = 1; col < A.cols; ++col)
    {
        int col_start = L.column_offset[col]; // col行开始的下标
        int col_minElmIdx = minElmIdx[col];

        // 计算对角线L[col, col]
        int len = col - col_minElmIdx + 1; // 表示在这一行col列之前有多少个需要被计算的元素
        double sum = 0;
        for (int i = 0; i < len; ++i)
        {
            sum += std::pow(L.elements[col_start + i], 2);
        }
        int diagIdx = L.column_offset[col + 1] - 1; // 下一行第一个元素的上一个元素就是这行的对角线元素
        double diag = std::sqrt(A.elements[diagIdx] - sum);
        L.elements[diagIdx] = diag;

        // 计算非对角线元素L[k, col], k = col+1, ... , n-1
#pragma omp parallel for private(len, sum)
        for (int k = col + 1; k < L.rows; ++k)
        {
            int k_minElmIdx = minElmIdx[k];
            int diff;
            int flag1, flag2;
            if (k_minElmIdx > col_minElmIdx)
            {
                len = col - k_minElmIdx + 1;
                diff = k_minElmIdx - col_minElmIdx;
                flag1 = 0;
                flag2 = 1;
            }
            else
            {
                len = col - col_minElmIdx + 1;
                diff = col_minElmIdx - k_minElmIdx;
                flag1 = 1;
                flag2 = 0;
            }
            if (len <= 0) // len <= 0, 表示这一行无需计算
            {
                continue;
            }

            sum = 0;
            int k_start = L.column_offset[k]; // k行开始的下标
            for (int i = 0; i < len; ++i)
            {
                sum += L.elements[k_start + diff * flag1 + i] * L.elements[col_start + diff * flag2 + i];
            }
            int idx = L.column_offset[k + 1] - k + col - 1;
            L.elements[idx] = (A.elements[idx] - sum) / diag;
        }
    }
}