#include <Mesh.h>
#include <Viewer.h>
#include <cube.h>
#include <sphere.h>
#include <iostream>
#include <cstring>

int main(int argc, char *argv[])
{
    Mesh mesh;
    int subdiv;

    if (argc < 2)
    {
        // 如果没有输入参数，默认显示细分为 10 的球体
        subdiv = 10;
        load_sphere(mesh, subdiv);
    }
    else if (argc == 2)
    {
        // 如果参数数量为 2，显示用法提示信息
        std::cerr << "Usage: ./test_viewer {cube/sphere} subdiv " << std::endl;
        return 0;
    }
    else if (argc > 2)
    {
        // 获取细分参数并解析为整数
        subdiv = std::stoi(argv[2]);

        // 根据第一个参数决定加载立方体或球体
        if (std::strcmp(argv[1], "cube") == 0)
        {
            load_cube(mesh, subdiv);
        }
        else if (std::strcmp(argv[1], "sphere") == 0)
        {
            load_sphere(mesh, subdiv);
        }
        else
        {
            std::cerr << "Invalid shape. Use 'cube' or 'sphere'." << std::endl;
            return 1;
        }
    }
    Viewer viewer(1200, 800, "3D Viewer");
    viewer.initialize();

    viewer.displayMesh(mesh); // 设置要显示的网格
    viewer.run();             // 开始渲染

    return 0;
}
