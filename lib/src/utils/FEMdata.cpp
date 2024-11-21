#include <FEMdata.h>
#include <TArray.h>
#include <CSRMatrix.h>
#include <Mesh.h>
#include <vec3.h>
#include <fem.h>
#include <systemSolve.h>
#include <timer.h>

FEMData::FEMData(int subdiv, MeshType meshtype, double (*func)(Vec3 pos))
    : mesh(subdiv, meshtype), A(mesh), u(mesh.vertex_count(), 0.0), B(mesh.vertex_count())
{
    Timer t;
    Vec b(mesh.vertex_count());
    for (size_t i = 0; i < mesh.vertex_count(); ++i)
    {
        b[i] = func(mesh.vertices[i]);
    }

    t.start();
    CSRMatrix M(mesh);
    buildMassMatrix(M);
    buildStiffnessMatrix(A);
    addMassToStiffness(A, M);
    t.stop();
    std::cout << "建立矩阵耗时: " << t.elapsedMilliseconds() << "ms" << std::endl;

    M.MVP(b, B);
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