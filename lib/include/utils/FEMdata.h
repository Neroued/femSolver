#pragma once

#include <TArray.h>
#include <CSRMatrix.h>
#include <Mesh.h>
#include <vec3.h>

class FEMData
// 存储求解-\Delta u + u = f的相关结果
{
public:
    Mesh mesh;
    CSRMatrix A;
    Vec u;
    Vec B;

    FEMData(int subdiv, MeshType meshtype, double (*func)(Vec3 pos));
};

