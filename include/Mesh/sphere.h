#pragma once
#include <vec3.h>
#include <cube.h>
#include <Mesh.h>

#ifndef SPHERE
#define SPHERE 2
#endif

int load_sphere(Mesh &m, int subdiv)
{
    load_cube(m, subdiv);

    Timer t;
    t.start();
    for (int i = 0; i < (int)m.vertex_count(); ++i)
    {
        m.vertices[i] = normalized(m.vertices[i]);
    }
    t.stop();
    std::cout << "生成球面用时: " << t.elapsedMilliseconds() << "ms" << std::endl;;
    return 0;
}

Mesh::Mesh(int subdiv, MeshType meshtype)
{
    if (meshtype == CUBE)
    {
        load_cube(*this, subdiv);
    }
    else if (meshtype == SPHERE)
    {
        load_sphere(*this, subdiv);
    }
}