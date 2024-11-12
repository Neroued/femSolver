#include <iostream>
#include <random>
#include <stdexcept>
#include <sparseMatrix.h>
#include <TArray.h>
#include <systemSolve.h>
#include <string>
#include <timer.h>

// 生成稀疏矩阵的函数，生成条件数较差的矩阵
void generateStiffnessMatrix(SparseMatrix &matrix, int size)
{
    // 刚度矩阵通常是对称的，这里我们修改对角线元素，使其值在一个较大的范围内变化
    int nonZeroElements = 3 * size - 2; // 主对角线 + 上下相邻对角线

    matrix.cols = matrix.rows = size;
    matrix.nnz = nonZeroElements;
    delete[] matrix.cooefs;
    matrix.cooefs = new struct Cooef[nonZeroElements];

    int index = 0;

    // 设置主对角线和相邻对角线元素
    for (int i = 0; i < size; ++i)
    {
        // 修改主对角线元素，使其值在 [1, large_value] 之间变化
        // 我们可以使用指数函数或其他方式来增加条件数
        double large_value = 1e4;                                                      // 设置一个大的值，增大条件数
        double diagonalValue = 1.0 + (large_value - 1.0) * ((double)i / (size - 1)); // 从1到large_value线性增长

        matrix.cooefs[index++] = {i, i, diagonalValue};

        // 上对角线元素，保持为 -1.0
        if (i < size - 1)
        {
            matrix.cooefs[index++] = {i, i + 1, -1.0};
            matrix.cooefs[index++] = {i + 1, i, -1.0}; // 对称性
        }
    }
}

void generateMassMatrix(SparseMatrix &matrix, int size)
{
    // 质量矩阵通常是对角矩阵，这里保持不变
    matrix.cols = matrix.rows = size;
    matrix.nnz = size;
    delete[] matrix.cooefs;
    matrix.cooefs = new struct Cooef[size];

    for (int i = 0; i < size; ++i)
    {
        // 主对角线，假设每个元素为 1.0
        matrix.cooefs[i] = {i, i, 1.0};
    }
}

// 生成向量的函数
Vec generateVector(int size)
{
    Vec vec(size, 0.0); // 初始化为0

    std::default_random_engine generator;
    std::uniform_real_distribution<double> valDist(1.0, 10.0);

    for (int i = 0; i < size; ++i)
    {
        vec[i] = valDist(generator);
    }

    return vec;
}

// 测试
void testSolve(int choice)
{
    int matrixSize = 1000; // 矩阵的维度
    int iterMax = INT32_MAX;
    double tol = 1e-7;

    // 生成刚度矩阵 M 和质量矩阵 S
    SparseMatrix M;
    generateStiffnessMatrix(M, matrixSize);
    SparseMatrix S;
    generateMassMatrix(S, matrixSize);

    Vec B = generateVector(matrixSize);

    Vec solution1(matrixSize, 1);

    Timer t;

    t.start();
    bool f1 = decentGradientSolve(M, S, B, solution1, tol, iterMax);
    t.stop();
    double t1 = t.elapsedMilliseconds();

    Vec solution2(matrixSize, 1);

    t.start();
    bool f2 = conjugateGradientSolve(M, S, B, solution2, tol, iterMax);
    t.stop();
    double t2 = t.elapsedMilliseconds();

    std::cout << "decentGraident用时: " << t1 << "ms" << " 收敛性: " << f1 << std::endl;
    std::cout << " B - Au norm: " << (B - MVP(M, solution1) - MVP(S, solution1)).norm() << std::endl;
    std::cout << "conjugateGradient用时: " << t2 << "ms" << " 收敛性: " << f2 << std::endl;
    std::cout << " B - Au norm: " << (B - MVP(M, solution2) - MVP(S, solution2)).norm() << std::endl;
    std::cout << " sol1 - sol2 norm: " << (solution1 - solution2).norm() << std::endl;
}

int main(int argc, char *argv[])
{
    int choice;
    if (argc < 2)
    {
        choice = 0;
    }
    else
    {
        choice = std::stoi(argv[1]);
    }

    testSolve(choice);
    return 0;
}
