#pragma once

#include <TArray.h>
#include <vec3.h>
#include <cstdint>

typedef int MeshType;

class Mesh
{
public:
    TArray<Vec3> vertices;        // 存储(x,y,z)点的坐标
    TArray<uint32_t> indices;     // 每三个顶点为一组表示三角形

    size_t vertex_count() const { return vertices.size; }
    size_t triangle_count() const { return indices.size / 3; }

    Mesh() = default;
    Mesh(int subdiv, MeshType meshtype);
};