#pragma once

#include <vec3.h>
#include <TArray.h>
#include <FEMatrix.h>
#include <systemSolve.h>
#include <iostream>
#include <timer.h>
#include <Mesh.h>

// 求解- \Delta u + u = f

static void massLoc(const Vec3 &AB, const Vec3 &AC, double *Mloc);  // 根据输入两个向量代表的三角形计算局部质量矩阵
static void stiffLoc(const Vec3 &AB, const Vec3 &AC, double *Sloc); // 同上，计算刚度矩阵

void buildMassMatrix(FEMatrix &M)
/* 根据网格建立质量矩阵
 * 对于每一个三角形，
 * 主对角线元素为 |ABC|/6 , 次对角线元素为 |ABC|/12
 * diag和offdiag均有n个元素
 * diag直接存储对角线元素
 * offdiag由于局部质量矩阵偏离对角线的元素仅有一种，因此每一个三角形仅增加一个元素
 */
{
    Mesh &mesh = M.m;
    // 根据每个三角形进行计算
    for (size_t t = 0; t < mesh.triangle_count(); ++t)
    {
        uint32_t a = mesh.indices[3 * t + 0];
        uint32_t b = mesh.indices[3 * t + 1];
        uint32_t c = mesh.indices[3 * t + 2];

        Vec3 A = mesh.vertices[a];
        Vec3 B = mesh.vertices[b];
        Vec3 C = mesh.vertices[c];

        Vec3 AB = B - A;
        Vec3 AC = C - A;

        // 局部质量矩阵，9个元素，其中只有2个不同的值, 第一个值为对角线元素，第二个值为非对角线元素
        double Mloc[2];
        massLoc(AB, AC, Mloc);

        M.diag[a] += Mloc[0];
        M.diag[b] += Mloc[0];
        M.diag[c] += Mloc[0];

        M.offdiag[t] = Mloc[1];
    }
}

static void massLoc(const Vec3 &AB, const Vec3 &AC, double *Mloc)
/* 根据输入的向量AB和AC计算局部质量矩阵
 * 首先计算三角形面积，公式为 |ABC| = 0.5 * |AB x AC|
 */
{
    double S_ABC = 0.5 * norm(cross(AB, AC));
    Mloc[0] = S_ABC / 6.0;
    Mloc[1] = S_ABC / 12.0;
}

void buildStiffnessMatrix(FEMatrix &S)
/* 根据网格建立刚度矩阵
 * 与质量矩阵区别在于
 * 局部刚度矩阵有6个不同元素，因此每个三角形需要占用三个offdiag的空间
 */
{
    Mesh &mesh = S.m;
    for (size_t t = 0; t < mesh.triangle_count(); ++t)
    {
        uint32_t a = mesh.indices[3 * t + 0];
        uint32_t b = mesh.indices[3 * t + 1];
        uint32_t c = mesh.indices[3 * t + 2];

        Vec3 A = mesh.vertices[a];
        Vec3 B = mesh.vertices[b];
        Vec3 C = mesh.vertices[c];

        Vec3 AB = B - A;
        Vec3 AC = C - A;

        // 局部刚度矩阵，9个元素，其中有6个不同的值, 前三个依次为对角线元素，后三个依次为S_AB, S_AC, S_BC
        double Sloc[6];
        stiffLoc(AB, AC, Sloc);

        S.diag[a] += Sloc[0];
        S.diag[b] += Sloc[1];
        S.diag[c] += Sloc[2];

        S.offdiag[3 * t + 0] = Sloc[3];
        S.offdiag[3 * t + 1] = Sloc[4];
        S.offdiag[3 * t + 2] = Sloc[5];
    }
}

static void stiffLoc(const Vec3 &AB, const Vec3 &AC, double *Sloc)
/* 计算局部刚度矩阵
 * BC = AC - AB
 * 存储顺序是S_AB, S_AC, S_BC
 */
{
    // 先计算好，减少浮点运算量
    double ABAB = norm2(AB);
    double ACAC = norm2(AC);
    double ABAC = dot(AB, AC);
    double mult = 0.5 / sqrt(ABAB * ACAC - ABAC * ABAC); // 1 / 4|ABC|
    ABAB *= mult;
    ACAC *= mult;
    ABAC *= mult;

    Sloc[0] = ACAC + ABAB - 2 * ABAC;
    Sloc[1] = ACAC;
    Sloc[2] = ABAB;
    Sloc[3] = ABAC - ACAC; // S_AB
    Sloc[4] = ABAC - ABAB; // S_AC
    Sloc[5] = -ABAC;       // S_BC
}

void addMassToStiffness(FEMatrix &S, FEMatrix &M)
// 将质量矩阵加到刚度矩阵，方便定义和使用统一的MVP
{
    Mesh &mesh = S.m;
    for (size_t i = 0; i < mesh.vertex_count(); ++i)
    {
        S.diag[i] += M.diag[i];
    }
    for (size_t i = 0; i < mesh.triangle_count(); ++i)
    {
        S.offdiag[3 * i + 0] += M.offdiag[i];
        S.offdiag[3 * i + 1] += M.offdiag[i];
        S.offdiag[3 * i + 2] += M.offdiag[i];
    }
}