#include <cube.h>
#include <iostream>
#include <fstream>
#include <json.hpp>
#include <sphere.h>

using json = nlohmann::json;

void save_json(Mesh &m);
void print_mesh_info(Mesh &m, const int subdiv);
int test_load_cube(Mesh &m, const int subdiv);
int test_load_sphere(Mesh &m, const int subdiv);

int main()
{
    int subdiv = 500;
    Mesh m;
    test_load_cube(m, subdiv);

    // test_load_sphere(m, subdiv);

    // print_mesh_info(m, subdiv);

    // save_json(m);

    return 0;
}

void save_json(Mesh &m)
{
    // 创建 JSON 对象
    json meshData;
    meshData["vertices"] = json::array();
    meshData["triangles"] = json::array();

    // 保存顶点信息到 JSON
    for (const auto &vec : m.vertices)
    {
        json vertex = {vec.x, vec.y, vec.z};
        meshData["vertices"].push_back(vertex);
    }

    // 保存三角形信息到 JSON
    for (size_t i = 0; i < m.indices.size; i += 3)
    {
        json triangle = {m.indices[i], m.indices[i + 1], m.indices[i + 2]}; // 假设 tri 是一个索引数组
        meshData["triangles"].push_back(triangle);
    }

    // 将 JSON 写入文件
    std::ofstream file("/mnt/e/myGlfw/mesh_data.json");
    if (file.is_open())
    {
        file << meshData.dump(4); // 格式化输出，缩进4个空格
        file.close();
        std::cout << "Mesh data saved to /mnt/e/myGlfw/mesh_data.json" << std::endl;
    }
    else
    {
        std::cerr << "Unable to open file for writing" << std::endl;
    }
}

void print_mesh_info(Mesh &m, const int subdiv)
{
    std::cout << "vertices: " << m.vertex_count() << std::endl;
    std::cout << "triangles: " << m.triangle_count() << std::endl;

    std::cout << "--------vertices--------" << std::endl;

    int i = 0;
    for (auto vec : m.vertices)
    {
        std::cout << vec << std::endl;
        ++i;
        if (i == (subdiv + 1) * (subdiv + 1))
        {
            std::cout << std::endl;
            i = 0;
        }
    }

    std::cout << "--------triangles--------" << std::endl;

    for (size_t i = 0; i < m.indices.size; i += 3)
    {
        std::cout << "[" << m.indices[i] << ", "
                  << m.indices[i + 1] << ", "
                  << m.indices[i + 2] << "]" << std::endl;
    }
}

int test_load_cube(Mesh &m, const int subdiv)
{
    Timer t;

    t.start();
    load_cube(m, subdiv);
    t.stop();

    std::cout << "生成总用时: " << t.elapsedMilliseconds() << "ms\n";

    return 0;
}

int test_load_sphere(Mesh &m, const int subdiv)
{
    Timer t;

    t.start();
    load_sphere(m, subdiv);
    t.stop();

    std::cout << "生成总用时: " << t.elapsedMilliseconds() << "ms\n";

    return 0;
}