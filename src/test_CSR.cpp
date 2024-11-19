#include <iostream>
#include <CSRMatrix.h>
#include <Mesh.h>
#include <cube.h>
#include <sphere.h>
#include <fem.h>
#include <time.h>
#include <systemSolve.h>
#include <omp.h>
#include <string.h>
#include <cmath>

void printinfo(CSRMatrix &csr);

static double test_f(Vec3 pos)
{
    using namespace std;
    
    double x = pos[0];
    double y = pos[1];

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
    Mesh mesh;
    int subdiv;
    int res;
    int threads;
    if (argc > 3 && strncmp(argv[1], "cube", 4) == 0)
    {
        subdiv = atoi(argv[2]);
        res = load_cube(mesh, subdiv);
        threads = atoi(argv[3]);
    }
    else if (argc > 3 && strncmp(argv[1], "sphere", 5) == 0)
    {
        subdiv = atoi(argv[2]);
        res = load_sphere(mesh, subdiv);
        threads = atoi(argv[3]);
    }
    else
    {
        load_cube(mesh, subdiv);
        threads = 2;
    }
    omp_set_num_threads(threads);
    Timer t;

    t.start();

    CSRMatrix M(mesh);
    CSRMatrix S(mesh);
    t.stop();
    std::cout << "用时1: " << t.elapsedMilliseconds() << "ms" << std::endl;
    t.start();
    buildStiffnessMatrix(S);
    buildMassMatrix(M);
    addMassToStiffness(S, M);
    t.stop("建立矩阵用时");

    // printinfo(csr);

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
    M.MVP(f, B);

    double rel_error;
    int iter;
    std::cout << "开始求解";
    t.start();
    conjugateGradientSolve(S, B, u, r, p, Ap, &rel_error, &iter, 1e-6, 100000);
    t.stop();
    std::cout << "用时: " << t.elapsedMilliseconds() << "ms" << std::endl;

    std::cout << "u[n - 1]: " << u[n - 1] << std::endl;
}

void printinfo(CSRMatrix &csr)
{
    std::cout << "elements: " << std::endl;
    int i = 0;
    for (size_t j = 0; j < csr.row_offset.size - 1; ++j)
    {
        auto r = csr.row_offset[j];
        int len = csr.row_offset[j + 1] - r;
        std::cout << "row " << i++ << " : [";

        for (int t = 0; t < len - 1; ++t)
        {
            std::cout << csr.elements[r + t] << ", ";
        }
        std::cout << csr.elements[r + len - 1] << "]" << std::endl;
        ;
    }

    std::cout << "elm_idx: " << std::endl;
    i = 0;
    for (size_t j = 0; j < csr.row_offset.size - 1; ++j)
    {
        auto r = csr.row_offset[j];
        int len = csr.row_offset[j + 1] - r;
        std::cout << "row " << i++ << " : [";

        for (int t = 0; t < len - 1; ++t)
        {
            std::cout << csr.elm_idx[r + t] << ", ";
        }
        std::cout << csr.elm_idx[r + len - 1] << "]" << std::endl;
        ;
    }
    std::cout << "row_offset" << csr.row_offset << std::endl;
}