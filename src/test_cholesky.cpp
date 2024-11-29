#include <cholesky.h>
#include <iostream>
#include <CSRMatrix.h>
#include <Mesh.h>
#include <fem.h>

#include <cmath>
#include <vector>
#include <iostream>
#include <timer.h>

#include <Eigen/Sparse>
#include <Eigen/SparseCholesky>

void ConvertToEigenMatrix(const CSRMatrix &csr_matrix, Eigen::SparseMatrix<double> &eigen_matrix)
{
    typedef Eigen::Triplet<double> Triplet;
    std::vector<Triplet> tripletList;
    tripletList.reserve(csr_matrix.elements.size);

    for (size_t i = 0; i < csr_matrix.row_offset.size - 1; ++i)
    {
        size_t row_start = csr_matrix.row_offset[i];
        size_t row_end = csr_matrix.row_offset[i + 1];
        for (size_t idx = row_start; idx < row_end; ++idx)
        {
            size_t col = csr_matrix.elm_idx[idx];
            double value = csr_matrix.elements[idx];
            tripletList.emplace_back(static_cast<int>(i), static_cast<int>(col), value);
        }
    }

    eigen_matrix.resize(csr_matrix.rows, csr_matrix.cols);
    eigen_matrix.setFromTriplets(tripletList.begin(), tripletList.end());
}

int main()
{
    // 定义一个 3x3 的正定对称矩阵
    const int matrix_size = 3;

    CSRMatrix A(matrix_size);

    // 构造矩阵数据：正定对称矩阵
    // 示例矩阵：
    // | 4  12 -16 |
    // | 12 37 -43 |
    // | -16 -43 98 |
    A.row_offset = {0, 3, 6, 9};             // 每行非零元素的偏移量
    A.elm_idx = {0, 1, 2, 0, 1, 2, 0, 1, 2}; // 非零元素的列索引
    A.elements = {
        4.0, 12.0, -16.0,  // 第一行
        12.0, 37.0, -43.0, // 第二行
        -16.0, -43.0, 98.0 // 第三行
    };

    // 输出原始矩阵
    std::cout << "原始矩阵 A:" << std::endl;
    A.print();

    // 调用 Cholesky 分解函数
    Cholesky chol(A);

    std::cout << "分解结果矩阵 L:" << std::endl;
    chol.L.print();

    Vec b = {1, 2, 3};
    Vec x(3);

    chol.solve(b, x);
    std::cout << "x: " << x << std::endl;

    Timer t;
    int subdiv = 200;
    Mesh mesh(subdiv, SPHERE);
    CSRMatrix S(mesh);
    buildStiffnessMatrix(S, mesh);
    // t.start();
    // Cholesky chol2(S);
    // std::cout << "对于 n = " << S.rows;
    // t.stop(" 分解用时");
    // Vec B(S.rows, 1.0);
    // Vec X(S.rows);
    // t.start();
    // chol2.solve(B, X);
    // t.stop("求解用时");
    // chol2.L.print();
    // std::cout << "X: " << X << std::endl;

    Eigen::SparseMatrix<double> eigen_matrix;
    ConvertToEigenMatrix(S, eigen_matrix);
    t.start();
    Eigen::SimplicialLDLT<Eigen::SparseMatrix<double>> solver;
    solver.compute(eigen_matrix);
    t.stop("Eigen分解用时");

    return 0;
}