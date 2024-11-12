#pragma once

#include <vec3.h>
#include <TArray.h>
#include <sparseMatrix.h>
#include <systemSolve.h>
#include <iostream>
#include <timer.h>
#include <Mesh.h>

// 求解- \Delta u + u = f

void buildMassMatrix(Mesh &mesh, SparseMatrix &M)
/* 根据网格建立质量矩阵
 * 主对角线元素为 |ABC|/6 , 次对角线元素为 |ABC|/12
 */
{
    Vec3 *A, *B, *C;
    size_t n = mesh.triangle_count();

    delete[] M.cooefs;
    M.cooefs = new struct Cooef[3 * n * n - 4 * n + 2]; // 共有3n^2 - 4n + 2个非零元素
    M.nnz = 3 * n * n - 4 * n + 2;

    for (size_t i = 0; i < n; ++i)
    {
        // 计算三角形面积
        A = &mesh.vertices[3 * i];
        B = &mesh.vertices[3 * i + 1];
        C = &mesh.vertices[3 * i + 2];

        Vec3 AB = *B - *A;
        Vec3 AC = *C - *A;

        double S = norm(cross(AB, AC)) * 0.5;
    }
}

void buildSniffnessMatrix(Mesh &mesh, SparseMatrix &S)
// 根据网格建立刚度矩阵
{
}