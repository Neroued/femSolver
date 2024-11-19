#include <NavierStokesSolver.h>
#include <iostream>
#include <Mesh.h>
#include <cube.h>
#include <sphere.h>
#include <systemSolve.h>
#include <timer.h>
#include <cmath>
#include <Viewer.h>

static double test_f(Vec3 pos, double omega_0 = 1.0, double sigma = 1.0)
{
    using namespace std;

    double x = pos[0];
    double y = pos[1];
    double z = pos[2];

    double dx = x;
    double dy = y;
    double r_squared = dx * dx + dy * dy;

    // 基于二维高斯分布生成涡量
    double omega = omega_0 * std::exp(-r_squared / (2.0 * sigma * sigma)) * (1.0 + 0.5 * std::cos(20.0 * x * y) * z);

    return omega;
}


int main()
{
    Timer t;
    t.start();
    int subdiv = 100;
    NavierStokesSolver Solver(subdiv, SPHERE);
    for (size_t i = 0; i < Solver.mesh.vertex_count(); ++i)
    {
        Solver.Omega[i] = test_f(Solver.mesh.vertices[i], 1.0, 1.5);
    }
    Solver.setZeroMean(Solver.Omega);
    int iter;
    double dt = 0.1;
    double nu = 1e-2;
    Viewer viewer(800, 600, "NS Solver");
    viewer.setupNS(Solver);
    viewer.runNS(dt, nu);

    t.stop("用时");
}