#include <iostream>
#include <random>
#include <stdexcept>
#include <sparseMatrix.h>
#include <TArray.h>
#include <systemSolve.h>
#include <string>

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
        double large_value = 1;                                                    // 设置一个大的值，增大条件数
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
    int matrixSize = 10; // 矩阵的维度
    int iterMax = 2147483647;

    // 生成刚度矩阵 M 和质量矩阵 S
    SparseMatrix M;
    generateStiffnessMatrix(M, matrixSize);
    SparseMatrix S;
    generateMassMatrix(S, matrixSize);
    Vec B = generateVector(matrixSize);

    // 输出生成的稀疏矩阵和向量
    // std::cout << "Stiffness Matrix M:" << std::endl;
    // M.print();
    // std::cout << "\nMass Matrix S:" << std::endl;
    // S.print();
    std::cout << "\nVector B:" << B << std::endl;

    Vec solution;
    if (choice)
    {
        solution = decentGradientSolve(M, S, B, iterMax);
    }
    else
    {
        solution = conjugateGradientSolve(M, S, B, iterMax);
    }

    // 输出结果
    std::cout << "\nSolution u:" << solution << std::endl;

    // 测试结果
    std::cout << "norm of B - Au : " << (B - (SMVP(M, solution) + SMVP(S, solution))).norm() << std::endl;
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
