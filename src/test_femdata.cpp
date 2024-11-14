#include <FEMdata.h>
#include <vec3.h>
#include <string.h>
#include <iostream>
#include <Mesh.h>
#include <timer.h>
#include <Viewer.h>


double test_f(Vec3 pos)
{
    double x = pos[0];
    double y = pos[1];
    return (5 * pow(x, 4) * y - 10 * pow(x, 2) * pow(y, 3) + pow(y, 5));
}

int main(int argc, char *argv[])
{
    int subdiv;
    MeshType meshtype;
    if (argc > 2 && strncmp(argv[1], "cube", 4) == 0)
    {
        subdiv = atoi(argv[2]);
        meshtype = CUBE;
    }
    else if (argc > 2 && strncmp(argv[1], "sphere", 5) == 0)
    {
        subdiv = atoi(argv[2]);
        meshtype = SPHERE;
    }
    else
    {
        std::cout << "Usage: " << argv[0] << " {cube/sphere{} subdiv" << std::endl;
        return -1;
    }
    Timer t;
    t.start();
    FEMData femdata(subdiv, meshtype, test_f);
    t.stop();
    std::cout << "创建FEMData共耗时: " << t.elapsedMilliseconds() << "ms" << std::endl;

    Viewer viewer(1000, 800, "FEMData Viewer");
    viewer.setupFEMData(femdata);
    viewer.runFEMData();

    return 0;
}