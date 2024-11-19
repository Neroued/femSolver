#pragma once

#include <Mesh.h>
#include <TArray.h>
#include <vec3.h>
#include <cube.h>
#include <sphere.h>
#include <CSRMatrix.h>
#include <fem.h>
#include <systemSolve.h>
#include <iostream>

class NavierStokesSolver
/* 求解 NS 方程: (M + dt * nu * S) * Omega^{t+dt} = dt * M * Omega^t + dt * T(Omega^t, Psi^t)
 *                                  -S * Psi^t = M * Omega^t
 * 每一次对于时间的迭代, A = (M + dt * nu * S) 这一部分是不变的
 * 因此相对于每一次都使用迭代的方法，计算A的逆以加快计算 Omega^{t+dt} 是可以接受的
 * 同样，可以计算S的逆
 * 1. 计算 Psi^t
 * 2. 计算 T(Omega^t, Psi^t)
 * 3. 求解 Omega^{t+dt}
 */
{
public:
    Mesh mesh;
    CSRMatrix M, S, A;
    Vec Omega;
    Vec MOmega;
    Vec Psi;
    Vec T;
    Vec r;
    Vec p;
    Vec Ap;
    double t; // 时间
    double tol;

    NavierStokesSolver(int subdiv, MeshType meshtype);
    ~NavierStokesSolver() = default;

    void computeStream(int *iter);
    void setZeroMean(Vec &x);
    void computeTransport();
    void timeStep(double dt, double nu);
};

NavierStokesSolver::NavierStokesSolver(int subdiv, MeshType meshtype)
    : mesh(subdiv, meshtype), M(mesh), S(mesh), A(mesh), Omega(M.rows, 0), MOmega(M.rows, 0), Psi(M.rows, 0), T(M.rows, 0), r(M.rows, 0), p(M.rows, 0), Ap(M.rows, 0)
{
    t = 0;
    tol = 1e-5;
    buildMassMatrix(M);
    buildStiffnessMatrix(S);
}

void NavierStokesSolver::computeStream(int *iter)
{
    M.MVP(Omega, MOmega);
    MOmega.scaleInPlace(-1.0);
    double rel_error;

    int iterMax = 100000;
    conjugateGradientSolve(S, MOmega, Psi, r, p, Ap, &rel_error, iter, tol, iterMax);
}

void NavierStokesSolver::setZeroMean(Vec &x)
{
    double mean = x.sum() / (double)x.size;
    for (size_t t = 0; t < x.size; ++t)
    {
        x[t] -= mean;
    }
}

void NavierStokesSolver::computeTransport()
// 看不懂怎么算，问老师
{
    T.setAll(0);

    for (size_t t = 0; t < mesh.triangle_count(); ++t)
    {
        uint32_t a = mesh.indices[3 * t + 0];
        uint32_t b = mesh.indices[3 * t + 1];
        uint32_t c = mesh.indices[3 * t + 2];

        double sum = Omega[a] + Omega[b] + Omega[c];
        T[a] += sum * (Psi[b] - Psi[c]);
        T[b] += sum * (Psi[c] - Psi[a]);
        T[c] += sum * (Psi[a] - Psi[b]);
    }

    for (size_t t = 0; t < mesh.vertex_count(); ++t)
    {
        T[t] *= 1.0 / 12;
    }
}

void NavierStokesSolver::timeStep(double dt, double nu)
{
    int iter1, iter2;
    double rel_error;
    Timer timer;
    timer.start();

    computeStream(&iter1);

    computeTransport();
    M.MVP(Omega, MOmega);
    blas_axpby(1.0, MOmega, dt, T, MOmega);
    // MOmega = MOmega + dt * T;
    blas_addMatrix(S, dt * nu, M, A);

    conjugateGradientSolve(A, MOmega, Omega, r, p, Ap, &rel_error, &iter2, tol, 1000);
    setZeroMean(Omega);
    t += dt;
    std::cout << "Iter 1: " << iter1 << ", Iter2: " << iter2;
    timer.stop(" time");
}