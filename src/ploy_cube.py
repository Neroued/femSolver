import json
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d.art3d import Poly3DCollection, Line3DCollection
import numpy as np

# 读取 JSON 文件
with open("./build/mesh_data.json", "r") as file:
    mesh_data = json.load(file)

# 获取顶点和三角形数据
vertices = np.array(mesh_data["vertices"])
triangles = mesh_data["triangles"]

# 设置观察者的位置
observer = np.array([2, 2, 2])  # 位置可以根据需要调整

# 定义一个函数来判断边是否可见
def is_visible(v1, v2, normal):
    view_vector = observer - (v1 + v2) / 2
    return np.dot(view_vector, normal) < 0

# 创建一个 3D 图形
fig = plt.figure()
ax = fig.add_subplot(111, projection="3d")

# 绘制每个三角形及其边
for tri in triangles:
    tri_vertices = vertices[tri]
    poly = Poly3DCollection([tri_vertices], alpha=0.2, edgecolor='k')
    ax.add_collection3d(poly)

    # 计算三角形的法向量
    v1, v2, v3 = tri_vertices
    normal = np.cross(v2 - v1, v3 - v1)
    normal = normal / np.linalg.norm(normal)  # 归一化法向量

    # 绘制边，根据可见性选择线型
    edges = [(v1, v2), (v2, v3), (v3, v1)]
    for edge in edges:
        v1, v2 = edge
        linestyle = "-" if is_visible(v1, v2, normal) else "--"
        ax.plot([v1[0], v2[0]], [v1[1], v2[1]], [v1[2], v2[2]], linestyle=linestyle, color="black")

# 设置坐标轴范围
all_x = vertices[:, 0]
all_y = vertices[:, 1]
all_z = vertices[:, 2]

ax.set_xlim([min(all_x), max(all_x)])
ax.set_ylim([min(all_y), max(all_y)])
ax.set_zlim([min(all_z), max(all_z)])

# 设置轴标签
ax.set_xlabel("X")
ax.set_ylabel("Y")
ax.set_zlabel("Z")


# 显示图形
plt.savefig("/mnt/e/myGlfw/cube_plot.png")
