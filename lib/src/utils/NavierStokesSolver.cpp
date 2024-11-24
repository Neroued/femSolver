#include <NavierStokesSolver.h>
#include <Mesh.h>
#include <TArray.h>
#include <vec3.h>
#include <CSRMatrix.h>
#include <fem.h>
#include <systemSolve.h>
#include <iostream>
#include <timer.h>

NavierStokesSolver::NavierStokesSolver(int subdiv, MeshType meshtype)
    : mesh(subdiv, meshtype, true), M(mesh), S(mesh), A(mesh), Omega(M.rows, 0), MOmega(M.rows, 0), Psi(M.rows, 0), T(M.rows, 0), r(M.rows, 0), p(M.rows, 0), Ap(M.rows, 0)//, multigrid(mesh, buildStiffnessMatrix)
{
    t = 0;
    tol = 1e-6;
    buildMassMatrix(M);
    buildStiffnessMatrix(S);
    vol = M.elements.sum();
}

void NavierStokesSolver::computeStream(int *iter)
{
    M.MVP(Omega, MOmega);
    MOmega.scaleInPlace(-1.0);
    setZeroMean(MOmega);
    Psi.setAll(0.0);
    // multigrid.solve(MOmega, Psi);
    double rel_error;
    int iterMax = 10000;
    Psi.setAll(0.0);
    conjugateGradientSolve(S, MOmega, Psi, r, p, Ap, &rel_error, iter, tol, iterMax);
}

void NavierStokesSolver::setZeroMean(Vec &x)
// 对MOmega进行zeromean操作，避免MOmega在ker(S)中
{
    double mean = x.sum() / (double)x.size;

    for (size_t t = 0; t < x.size; ++t)
    {
        x[t] -= mean;
    }
}

void NavierStokesSolver::computeTransport()
{
    T.setAll(0.0);

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

    for (size_t t = 0; t < T.size; ++t)
    {
        T[t] *= 1.0 / 6;
    }
}

void NavierStokesSolver::timeStep(double dt, double nu)
{
    int iter1 = 0, iter2;
    double rel_error;
    Timer timer;
    timer.start();

    computeStream(&iter1);
    computeTransport();
    M.MVP(Omega, p);
    blas_axpby(1.0, p, dt, T, MOmega);
    // MOmega = MOmega + dt * T;
    blas_addMatrix(S, dt * nu, M, A);
    // A = M + dt * nu * S

    conjugateGradientSolve(A, MOmega, Omega, r, p, Ap, &rel_error, &iter2, tol, 1000);
    setZeroMean(Omega);
    t += dt;
    std::cout << "Iter 1: " << iter1 << ", Iter2: " << iter2;
    timer.stop(" time");
}