#include <vec3.h>
#include <cube.h>

void load_sphere(Mesh &m, int subdiv)
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
}