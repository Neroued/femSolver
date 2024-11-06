#pragma once

#include <Mesh.h>
#include <vector>
#include <timer.h>
#include <iostream>

/* 生成立方体网格
 * 对于有n个子分割的网格
 * 正方体网格中有6n^2+2个顶点, 12n^2个三角形
 * 1. 生成顶点，同时使用hash表记录每个顶点的对应下标，生成一个不包含重复点的顶点集合 vertex_index_map
 * 2. 同时生成一个有重复点时下标与无重复点时下标的hash表 dupToNoDupIndex
 * 3. 根据dupToNoDupIndex创建三角形
 *
 */

int load_cube(Mesh &m, const int subdiv)
{
    Timer timer;
    timer.start();

    int n = subdiv + 1;

    Vec3 Axis_x = {1, 0, 0};
    Vec3 Axis_y = {0, 1, 0};
    Vec3 Axis_z = {0, 0, 1};

    std::vector<std::vector<Vec3>> r = {{Axis_z, Axis_x, Axis_y}, {Axis_x, Axis_y, Axis_z}, {Axis_y, Axis_x, Axis_z}};

    std::unordered_map<Vec3, int> vertex_index_map;
    vertex_index_map.reserve(6 * subdiv * subdiv + 2); // 为哈希表预分配空间
    int *dupToNoDupIndex = new int[6 * n * n];

    m.vertices.resize(6 * subdiv * subdiv + 2); // 总共6(subdiv+1)^2个点，实际不重复的点有6*subdiv^2+2个

    int t = 0; // t 表示存在重复点的下标
    int p = 0; // p 表示不重复点的下标
    for (auto v : r)
    {
        Vec3 a = v[0];
        Vec3 b = v[1];
        Vec3 c = v[2];
        for (float f = 0.f; f < 1.9f; f += 1)
        {
            for (int i = 0; i < n; ++i)
            {
                for (int j = 0; j < n; ++j)
                {
                    Vec3 tmp;
                    tmp = a * f + b * (float(i) / subdiv) + c * (float(j) / subdiv);

                    auto it = vertex_index_map.find(tmp);
                    if (it == vertex_index_map.end()) // 若这个点是首次出现
                    {
                        
                        vertex_index_map[tmp] = p; // 存储这个点的位置为p
                        m.vertices[p] = tmp;
                        dupToNoDupIndex[t] = p;
                        ++p;
                    }
                    else
                    {
                        dupToNoDupIndex[t] = it->second; // 将重复点的下标对应到不重复点的下标
                    }

                    ++t;
                }
            }
        }
    }
    timer.stop();

    std::cout << "生成网格用时: " << timer.elapsedMilliseconds() << "ms\n";

    timer.start();
    // 生成三角形
    m.triangles.resize(12 * subdiv * subdiv); // 共计12n^2个三角形

    t = 0;
    for (int i = 0; i < 6; ++i)
    {
        int n = i * (subdiv + 1) * (subdiv + 1);
        for (int j = 0; j < subdiv; ++j)
        {
            int p = n + j * (subdiv + 1);
            for (int k = 0; k < subdiv; ++k)
            {
                m.triangles[t++] = {dupToNoDupIndex[p + k], dupToNoDupIndex[p + k + 1], dupToNoDupIndex[p + k + subdiv + 1]};
                m.triangles[t++] = {dupToNoDupIndex[p + k + 1],
                                    dupToNoDupIndex[p + k + subdiv + 2],
                                    dupToNoDupIndex[p + k + subdiv + 1]};
            }
        }
    }

    delete[] dupToNoDupIndex;
    timer.stop();

    std::cout << "生成三角形用时: " << timer.elapsedMilliseconds() << "ms\n";

    return 0;
}