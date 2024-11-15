#pragma once

#include <TArray.h>
#include <FEMatrix.h>
#include <Mesh.h>
#include <vec3.h>
#include <cube.h>
#include <sphere.h>
#include <fem.h>
#include <systemSolve.h>
#include <timer.h>

class FEMData
// 存储求解-\Delta u + u = f的相关结果
{
public:
    Mesh mesh;
    FEMatrix A;
    Vec u;
    Vec B;

    FEMData(int subdiv, MeshType meshtype, double (*func)(Vec3 pos));
};

FEMData::FEMData(int subdiv, MeshType meshtype, double (*func)(Vec3 pos))
: mesh(subdiv, meshtype), A(mesh, FEMatrix::P1_Stiffness), u(mesh.vertex_count(), 0.0), B(mesh.vertex_count())
{
    Timer t;
    for (size_t i = 0; i < mesh.vertex_count(); ++i)
    {
        B[i] = func(mesh.vertices[i]);
    }
    
    t.start();
    FEMatrix M(mesh, FEMatrix::P1_Mass);
    buildMassMatrix(M);
    buildStiffnessMatrix(A);
    addMassToStiffness(A, M);
    t.stop();
    std::cout << "建立矩阵耗时: " << t.elapsedMilliseconds() << "ms" << std::endl;

    for (size_t i = 0; i < M.diag.size; ++i)
    {
        B[i] *= M.diag[i];
    }

    int n = mesh.vertex_count();
    double tol = 1e-6;
    int iter;
    double rel_error;
    int iterMax = 10000;
    Vec r(n);
    Vec p(n);
    Vec Ap(n);
    
    t.start();
    conjugateGradientSolve(A, B, u, r, p, Ap, &rel_error, &iter, tol, iterMax);
    t.stop();
    std::cout << "conjugateGradient求解耗时: " << t.elapsedMilliseconds() << "ms" << std::endl;
}