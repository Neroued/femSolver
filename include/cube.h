#pragma once

#include <Mesh.h>
#include <vector>
#include <timer.h>
#include <iostream>

static void load_cube_vertices(Mesh &m, const int subdiv); // 根据subdiv分割顶点

static void load_cube_triangles(Mesh &m, const int subdiv); // 在分割顶点之后，填入三角形

static void hash_mesh(Mesh &m, const int subdiv); // 使用hash表，去除其中重复的点并修改三角形

int load_cube(Mesh &m, const int subdiv); // 生成一个cube的网格

static void load_cube_vertices(Mesh &m, const int subdiv)
{
    Vec3 zero = {0, 0, 0};
    Vec3 Axis_x = {1, 0, 0};
    Vec3 Axis_y = {0, 1, 0};
    Vec3 Axis_z = {0, 0, 1};

    std::vector<std::vector<Vec3>> r = {{Axis_z, Axis_x, Axis_y}, {Axis_x, Axis_y, Axis_z}, {Axis_y, Axis_x, Axis_z}};

    // 不管重复点，6个面依次生成
    m.vertices.resize(6 * (subdiv + 1) * (subdiv + 1)); // 总共6(n+1)^2个点，实际不重复的点有6n^2+2个

    int t = 0;
    for (auto v : r)
    {
        Vec3 a = v[0];
        Vec3 b = v[1];
        Vec3 c = v[2];
        for (int i = 0; i < subdiv + 1; ++i)
        {
            for (int j = 0; j < subdiv + 1; ++j)
            {
                m.vertices[t] = a * 0.f + b * (float(i) / subdiv) + c * (float(j) / subdiv);
                m.vertices[t + 3 * (subdiv + 1) * (subdiv + 1)] = a * 1.f + b * (float(i) / subdiv) + c * (float(j) / subdiv);
                ++t;
            }
        }
    }
}

static void load_cube_triangles(Mesh &m, int subdiv)
{
    m.triangles.resize(12 * subdiv * subdiv); // 共计12n^2个三角形

    int t = 0;
    for (int i = 0; i < 6; ++i)
    {
        int n = i * (subdiv + 1) * (subdiv + 1);
        for (int j = 0; j < subdiv; ++j)
        {
            int p = n + j * (subdiv + 1);
            for (int k = 0; k < subdiv; ++k)
            {
                m.triangles[t++] = {p + k, p + k + 1, p + k + subdiv + 1};
                m.triangles[t++] = {p + k + 1,
                                    p + k + subdiv + 2,
                                    p + k + subdiv + 1};
            }
        }
    }
}

static void hash_mesh(Mesh &m, int subdiv)
{
    std::unordered_map<Vec3, int> vertex_index_map;
    TArray<Vec3> tmp(6 * subdiv * subdiv + 2);
    int t = 0;
    for (int i = 0; i < m.vertex_count(); ++i)
    {
        const Vec3 &v = m.vertices[i];
        if (vertex_index_map.find(v) == vertex_index_map.end())
        {
            vertex_index_map[v] = i;
            tmp.data[t++] = v;
        }
    }

    for (int i = 0; i < 6 * subdiv * subdiv + 2; ++i)
    {
        vertex_index_map[tmp.data[i]] = i;
    }

    for (int k = 0; k < 6; ++k)
    {
        for (int j = 0; j < 2 * subdiv * subdiv; ++j)
        {
            if (1)
            {
                TVec3<int> &T = m.triangles[j + k * 2 * subdiv * subdiv];
                for (int i = 0; i < 3; ++i)
                {
                    T[i] = vertex_index_map[m.vertices[T[i]]];
                }
            }
        }
    }

    m.vertices = tmp;
}

int load_cube(Mesh &m, const int subdiv)
{
    Timer t;

    t.start();
    load_cube_vertices(m, subdiv);

    load_cube_triangles(m, subdiv);
    t.stop();

    std::cout << "生成网格用时: " << t.elapsedMicroseconds() << "us\n";

    t.start();
    hash_mesh(m, subdiv);
    // optimized_hash_mesh(m, subdiv);
    t.stop();

    std::cout << "去重用时: " << t.elapsedMilliseconds() << "ms\n";

    return 0;
}