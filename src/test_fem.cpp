#include <iostream>
#include <Mesh.h>
#include <FEMatrix.h>
#include <fem.h>
#include <systemSolve.h>
#include <timer.h>
#include <string.h>

static double test_f(Vec3 pos)
{
    float x = pos[0];
    float y = pos[1];
    return (5 * pow(x, 4) * y - 10 * pow(x, 2) * pow(y, 3) + pow(y, 5));
}

static void fill_rhs(const Mesh &mesh, TArray<double> &f)
{
    for (size_t i = 0; i < mesh.vertex_count(); ++i)
    {
        f[i] = test_f(mesh.vertices[i]);
    }
}

int main(int argc, char *argv[])
{
    Timer t;
    Mesh mesh;
    int res = -1;
    int subdiv = 10;
    if (argc > 2 && strncmp(argv[1], "cube", 4) == 0)
    {
        subdiv = atoi(argv[2]);
        res = load_cube(mesh, subdiv);
    }
    else if (argc > 2 && strncmp(argv[1], "sphere", 5) == 0)
    {
        subdiv = atoi(argv[2]);
        res = load_sphere(mesh, subdiv);
    }
    else
    {
        load_cube(mesh, subdiv);
    }
    // 先根据subdiv生成网格

    t.start();

    // 根据网格生成FEMatrix
    FEMatrix M(mesh, FEMatrix::P1_Mass), S(mesh, FEMatrix::P1_Stiffness);

    buildMassMatrix(M);
    buildStiffnessMatrix(S);
    addMassToStiffness(S, M);

    t.stop();
    std::cout << "生成矩阵用时: " << t.elapsedMilliseconds() << "ms" << std::endl;

    // std::cout << "M.diag: " << M.diag << std::endl;
    // std::cout << "M.offdiag: " << M.offdiag << std::endl;
    // std::cout << "S.diag: " << S.diag << std::endl;
    // std::cout << "S.offdiag: " << S.offdiag << std::endl;
    //  S.offdiag.setAll(0.0);
    //  S.diag.setAll(3.0);

    int n = S.rows;
    std::cout << "S.rows: " << n << std::endl;
    Vec f(n, 0);
    Vec u(n, 0);
    Vec B(n, 0);
    Vec r(n);
    Vec Ar(n);
    Vec p(n);
    Vec Ap(n);

    fill_rhs(mesh, f);
    // std::cout << f << std::endl;

    M.MVP(f, B);
    // for (size_t i = 0; i < M.diag.size; ++i)
    // {
    //     B[i] = f[i] * M.diag[i];
    // }

    // std::cout << B << std::endl;
    //  f.setAll(1.0);
    double rel_error;
    int iter;
    std::cout << "开始求解" << std::endl;
    t.start();
    conjugateGradientSolve(S, B, u, r, p, Ap, &rel_error, &iter, 1e-6, 100000);
    // decentGradientSolve(S, B, u, r, Ar, &rel_error, &iter, 1e-6, 100000);
    t.stop();
    //  u.setAll(0);
    std::cout << "用时: " << t.elapsedMilliseconds() << "ms" << std::endl;

    std::cout << "u[n - 1]: " << u[n - 1] << std::endl;
}