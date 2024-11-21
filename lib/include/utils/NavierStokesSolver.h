#pragma once

#include <Mesh.h>
#include <TArray.h>
#include <vec3.h>
#include <CSRMatrix.h>
#include <fem.h>
#include <systemSolve.h>
#include <iostream>
#include <timer.h>

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
    double vol;

    NavierStokesSolver(int subdiv, MeshType meshtype);
    ~NavierStokesSolver() = default;

    void computeStream(int *iter);
    void setZeroMean(Vec &x);
    void computeTransport();
    void timeStep(double dt, double nu);
};
