#pragma once

#include <TArray.h>
#include <vec3.h>

class Mesh
{
public:
    TArray<Vec3> vertices;        // 存储(x,y,z)点的坐标
    TArray<TVec3<int>> triangles; // 用(0,1,2)表示一个三角形

    size_t vertex_count() { return vertices.size; }
    size_t triangle_count() { return triangles.size; }
};