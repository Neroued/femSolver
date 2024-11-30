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

#include <omp.h>

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

    CSRMatrix test_1(matrix_size);

    // 构造矩阵数据：正定对称矩阵
    // 示例矩阵：
    // | 4  12 -16 |
    // | 12 37 -43 |
    // | -16 -43 98 |
    test_1.row_offset = {0, 3, 6, 9};             // 每行非零元素的偏移量
    test_1.elm_idx = {0, 1, 2, 0, 1, 2, 0, 1, 2}; // 非零元素的列索引
    test_1.elements = {
        4.0, 12.0, -16.0,  // 第一行
        12.0, 37.0, -43.0, // 第二行
        -16.0, -43.0, 98.0 // 第三行
    };

    // 输出原始矩阵
    std::cout << "原始矩阵 test_1:" << std::endl;
    test_1.print();

    // 调用 Cholesky 分解函数
    Cholesky chol1;
    chol1.attach(test_1);
    chol1.compute();

    std::cout << "分解结果矩阵 L:" << std::endl;
    chol1.L.print();
    //  [[ 2.  0.  0.]
    //   [ 6.  1.  0.]
    //   [-8.  5.  3.]]

    CSRMatrix test_2(4);
    test_2.elements = {18, 22, 54, 42, 22, 70, 86, 62, 54, 86, 174, 134, 42, 62, 134, 106};
    test_2.row_offset = {0, 4, 8, 12, 16};
    test_2.elm_idx = {0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3};
    // 18  22   54   42
    // 22  70   86   62
    // 54  86  174  134
    // 42  62  134  106
    std::cout << "原始矩阵 test_2:" << std::endl;
    test_2.print();

    Cholesky chol2;
    chol2.attach(test_2);
    chol2.compute();

    std::cout << "分解结果矩阵 L:" << std::endl;
    chol2.L.print();
    // 4.24
    // 5.19   6.57
    // 12.73  3.05  1.65
    // 9.90   1.62  1.85  1.39

    CSRMatrix test_3(5);
    test_3.elements = {10, 2, 1, 2, 10, 3, 3, 10, 4, 4, 10, 5, 1, 5, 10};
    test_3.row_offset = {0, 3, 6, 9, 12, 15};
    test_3.elm_idx = {0, 1, 4, 0, 1, 2, 1, 2, 3, 2, 3, 4, 0, 3, 4};
    std::cout << "原始矩阵 test_3:" << std::endl;
    test_3.print();

    Cholesky chol3;
    chol3.attach(test_3);
    chol3.compute();

    std::cout << "分解结果矩阵 L:" << std::endl;
    chol3.L.print();

    Vec B = {1, 2, 3, 4, 5};
    Vec x(5);
    chol3.solve(B,x);
    std::cout << "x: " << x << std::endl;

    int subdiv = 2;
    double epsilon = 1e-6;
    Mesh mesh(subdiv, SPHERE);
    CSRMatrix test_4(mesh);
    buildStiffnessMatrix(test_4, mesh);

    // Cholesky chol4(test_4, epsilon);
    // chol4.compute();
    // chol4.L.print();



    // /*---分析效率---*/
    // omp_set_num_threads(4);
    // Timer t;
    // int subdiv = 100;
    // Mesh mesh(subdiv, SPHERE);
    // CSRMatrix S(mesh);
    // buildMassMatrix(S, mesh);

    // t.start();
    // Cholesky cholT(S);
    // std::cout << "对于 n = " << S.rows;
    // t.stop(" 分解用时");
    // subdiv = 40, n = 9602, threads = 4, mytime = 50ms, Eigen = 17ms

    // subdiv = 50, n = 15002, threads = 1, mytime = 232ms, Eigen = 33ms
    // subdiv = 50, n = 15002, threads = 2, mytime = 132ms, Eigen = 33ms
    // subdiv = 50, n = 15002, threads = 4, mytime = 100ms, Eigen = 33ms
    // subdiv = 50, n = 15002, threads = 8, mytime = 85ms, Eigen = 33ms
    // subdiv = 50, n = 15002, threads = 16, mytime = 96ms, Eigen = 33ms

    // subdiv = 100, n = 60002, threads = 1, mytime = 3926ms, Eigen = 300ms
    // subdiv = 100, n = 60002, threads = 2, mytime = 2307ms, Eigen = 300ms
    // subdiv = 100, n = 60002, threads = 4, mytime = 1669ms, Eigen = 300ms
    // subdiv = 100, n = 60002, threads = 8, mytime = 1361ms, Eigen = 300ms
    // subdiv = 100, n = 60002, threads = 16, mytime = 1248ms, Eigen = 300ms

    // subdiv = 200, n = 240002, threads = 4, mytime = 31733ms, Eigen = 3315ms
    // subdiv = 200, n = 240002, threads = 8, mytime = 27126ms, Eigen = 3315ms
    // subdiv = 200, n = 240002, threads = 16, mytime = 27511ms, Eigen = 3315ms

    // Eigen::SparseMatrix<double> eigen_matrix;
    // ConvertToEigenMatrix(S, eigen_matrix);
    // t.start();
    // Eigen::SimplicialLDLT<Eigen::SparseMatrix<double>> solver;
    // solver.compute(eigen_matrix);
    // t.stop("Eigen分解用时");

    // Vec B(S.rows, 1.0);
    // Vec X(S.rows);
    // t.start();
    // chol2.solve(B, X);
    // t.stop("求解用时");
    // chol2.L.print();
    // std::cout << "X: " << X << std::endl;

    return 0;
}