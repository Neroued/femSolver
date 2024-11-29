#include <SKRMatrix.h>
#include <TArray.h>
#include <Matrix.h>
#include <CSRMatrix.h>

SKRMatrix::SKRMatrix(CSRMatrix &A)
    : Matrix(A.rows, A.rows), column_offset(A.rows + 1, 0)
{
    // 输入A假定一定是对称正定的，寻找A每一行最左端的非零元素下标
    for (int i = 0; i < rows; ++i)
    {
        int left_elm_idx = A.elm_idx[A.row_offset[i]];
        int len = i - left_elm_idx + 1;
        column_offset[i + 1] = column_offset[i] + len;
    }
    elements.resize(column_offset[A.rows]);
};

void SKRMatrix::MVP(const Vec &x, Vec &y) const
{
    if (cols != x.size || cols != y.size)
    {
        throw std::invalid_argument("Size mismatch: The number of columns in the matrix does not match the size of the vector.");
    }
    y.setAll(0.0);

    int start;
    int len;
    for (int row = 0; row < rows; ++row)
    {
        start = column_offset[row];
        len = column_offset[row + 1] - start;
        for (int i = 0; i < len; ++i)
        {
            y[row] += x[row] * elements[start + i];
        }
    }
}

void SKRMatrix::convertFromCSR(const CSRMatrix &A)
{
    // 把CSR矩阵的下三角部分转换成SKR格式方便计算Cholesky
    // 要求是从A初始化的SKR矩阵
    for (int row = 0; row < rows; ++row)
    {
        int A_start = A.row_offset[row];
        int A_end = A.row_offset[row + 1];
        int SKR_start = column_offset[row];
        int SKR_len = column_offset[row + 1] - SKR_start;
        for (int i = A_start; (i < A_end) && (A.elm_idx[i] <= row); ++i)
        {
            elements[SKR_start + SKR_len - row - 1 + A.elm_idx[i]] = A.elements[i];
        }
    }
}