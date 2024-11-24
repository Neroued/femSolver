#include <NavierStokesSolver.h>
#include <iostream>
#include <Mesh.h>
#include <systemSolve.h>
#include <timer.h>
#include <cmath>
#include <Viewer.h>
#include <omp.h>
#include <MultiGrid.h>
#include <fem.h>

static double test_f(const Vec3 &pos, double omega_0 = 1.0, double sigma = 1.0)
{
    using namespace std;

    double x = pos[0];
    double y = pos[1];
    double z = pos[2];

    double r_squared = z * z;
    double theta = std::atan2(std::sqrt(x * x + y * y), z);
    double omega;

    // omega = 100 * z * std::exp(-50 * z * z) * (1 + 0.5 * cos(20 * theta));

    // 基于二维高斯分布生成涡量
    // omega = omega_0 * std::exp(-r_squared / (2.0 * sigma * sigma)) * (1.0 + 0.5 * std::cos(10.0 * theta) * z);

    omega = 100 * z * std::exp(-50 * r_squared) * (1.0 + 0.5 * std::cos(20 * theta));
    return omega;
}

int main(int argc, char *argv[])
{
    omp_set_num_threads(4);
    Timer t;
    t.start();
    int subdiv;
    MeshType mt;
    if (argc < 2)
    {
        subdiv = 8;
        mt = SPHERE;
    }
    else if (argc == 2)
    {
        std::cerr << "Usage: " << argv[0] << " {cube/sphere} subdiv " << std::endl;
        return 0;
    }
    else if (argc > 2)
    {
        subdiv = std::stoi(argv[2]);
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
    Mesh m0(subdiv, SPHERE, true);
    // std::cout << "subdiv: " << m0.subdiv << std::endl;
    // std::cout << "mt: " << m0.meshtype << std::endl;
    Vec b(m0.vertex_count());
    for (size_t i = 0; i < b.size; ++i)
    {
        b[i] = test_f(m0.vertices[i], 0.5, 1.5);
    }
    std::cout << "b initialized" << std::endl;
    MultiGrid mg(m0, buildMassMatrix);
    std::cout << "MultiGrid initialized" << std::endl;
    Vec x(b.size, 0);
    t.stop("用时");
    t.start();
    mg.solve(b, x);
    // std::cout << "b:" << b << std::endl;
    // std::cout << "mg.b1" << mg.b1 << std::endl;
    // Viewer viewer1(800, 600, "MultiGrid test");
    // viewer1.setupData(mg.m0, b);
    // viewer1.runData();
    // Viewer viewer2(800, 600, "MultiGrid test");
    // viewer2.setupData(mg.m1, mg.b1);
    t.stop("求解用时");
    // viewer2.runData();
}
