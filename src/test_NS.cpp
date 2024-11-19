#include <NavierStokesSolver.h>
#include <iostream>
#include <Mesh.h>
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
    double r_squared = z * z;

    // 基于二维高斯分布生成涡量
    double omega = omega_0 * std::exp(-r_squared / (2.0 * sigma * sigma)) * (1.0 + 0.5 * std::cos(20.0 * x * y) * z);

    return omega;
}


int main(int argc, char *argv[])
{
    Timer t;
    t.start();
    int subdiv;
    MeshType mt;
    if (argc < 2)
    {
        subdiv = 100;
        mt = SPHERE;
    }
    else if (argc == 2)
    {
        // 如果参数数量为 2，显示用法提示信息
        std::cerr << "Usage: " << argv[0] << " {cube/sphere} subdiv " << std::endl;
        return 0;
    }
    else if (argc > 2)
    {
        // 获取细分参数并解析为整数
        subdiv = std::stoi(argv[2]);

        // 根据第一个参数决定加载立方体或球体
        if (std::strcmp(argv[1], "cube") == 0)
        {
            mt = CUBE;
        }
        else if (std::strcmp(argv[1], "sphere") == 0)
        {
            mt = SPHERE;
        }
        else
        {
            std::cerr << "Invalid shape. Use 'cube' or 'sphere'." << std::endl;
            return 1;
        }
    }
    NavierStokesSolver Solver(subdiv, mt);
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