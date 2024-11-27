import numpy as np


class Mesh:
    def __init__(self):
        self.meshtype = None
        self.subdiv = 0
        self.vertices = []
        self.indices = []

    def vertex_count(self):
        return len(self.vertices)

    def triangles_count(self):
        return len(self.indices) // 3


def load_cube(mesh, subdiv):
    mesh.meshtype = "CUBE"
    mesh.subdiv = subdiv

    n = subdiv + 1

    faces = [
        {"axis": 0, "dir": 0},
        {"axis": 1, "dir": 0},
        {"axis": 2, "dir": 0},
        {"axis": 0, "dir": 1},
        {"axis": 1, "dir": 1},
        {"axis": 2, "dir": 1},
    ]

    total_vertices = 6 * n * n
    unique_vertices = 6 * subdiv * subdiv + 2

    # 使用一个字典模拟哈希表
    vertex_index_map = {}
    mesh.vertices = [None] * unique_vertices

    dup_to_nodup_index = np.zeros(total_vertices, dtype=int)

    t = 0  # 表示存在重复点的下标
    p = 0  # 表示不重复点的下标

    inv_subdiv = 1.0 / subdiv

    for face in faces:
        axis = face["axis"]
        dir = face["dir"]

        idx1 = (axis + 1) % 3
        idx2 = (axis + 2) % 3

        for i in range(n):
            for j in range(n):
                coords = [0, 0, 0]
                coords[axis] = dir * subdiv
                coords[idx1] = i
                coords[idx2] = j

                # 计算哈希值
                key = coords[0] | (coords[1] << 20) | (coords[2] << 40)

                if key not in vertex_index_map:  # 如果是首次出现
                    vertex_index_map[key] = p
                    dup_to_nodup_index[t] = p

                    # 计算顶点位置
                    fx = coords[0] * inv_subdiv * 2.0 - 1.0
                    fy = coords[1] * inv_subdiv * 2.0 - 1.0
                    fz = coords[2] * inv_subdiv * 2.0 - 1.0

                    mesh.vertices[p] = (fx, fy, fz)

                    p += 1
                else:
                    dup_to_nodup_index[t] = vertex_index_map[key]

                t += 1

    mesh.indices = [0] * (36 * subdiv * subdiv)
    t = 0
    face_vertex_offset = 0

    for face_idx in range(6):
        for i in range(subdiv):
            for j in range(subdiv):
                idx0 = face_vertex_offset + i * n + j
                idx1 = face_vertex_offset + i * n + j + 1
                idx2 = face_vertex_offset + (i + 1) * n + j
                idx3 = face_vertex_offset + (i + 1) * n + j + 1

                v0 = dup_to_nodup_index[idx0]
                v1 = dup_to_nodup_index[idx1]
                v2 = dup_to_nodup_index[idx2]
                v3 = dup_to_nodup_index[idx3]

                mesh.indices[t] = v0
                mesh.indices[t + 1] = v1
                mesh.indices[t + 2] = v2
                mesh.indices[t + 3] = v1
                mesh.indices[t + 4] = v3
                mesh.indices[t + 5] = v2
                t += 6

        face_vertex_offset += n * n

    return 0


class CSRMatrix:
    def __init__(self, mesh):
        """
        根据网格初始化CSR矩阵
        """
        self.mesh = mesh
        self.rows = len(mesh.vertices)
        self.cols = self.rows

        # 初始化 row_offset
        self.row_offset = np.ones(self.rows + 1, dtype=int)

        # 统计每个顶点的出现次数
        for t in range(mesh.triangle_count()):
            self.row_offset[mesh.indices[3 * t + 0]] += 1
            self.row_offset[mesh.indices[3 * t + 1]] += 1
            self.row_offset[mesh.indices[3 * t + 2]] += 1

        # 将数量转换为偏移量
        for i in range(1, self.rows):
            self.row_offset[i + 1] += self.row_offset[i]

        # 向后移动一位
        self.row_offset[1:] = self.row_offset[:-1]
        self.row_offset[0] = 0

        total_nonzeros = self.row_offset[self.rows]  # 最后一个元素为非零元素总数
        self.elements = np.zeros(total_nonzeros)  # 非零元素数组
        self.elm_idx = -np.ones(total_nonzeros, dtype=int)  # 列索引数组，初始化为 -1

        # 填充非零元素索引
        for t in range(mesh.triangle_count()):
            a, b, c = mesh.indices[3 * t : 3 * t + 3]
            triangle = [a, b, c]

            for current_vtx in triangle:
                for current_row in triangle:
                    offset = self.row_offset[current_row]
                    length = self.row_offset[current_row + 1] - offset

                    for i in range(length):
                        if self.elm_idx[offset + i] == current_vtx:
                            break  # 已经出现过，跳过
                        elif self.elm_idx[offset + i] == -1:  # 第一次出现，插入
                            self.elm_idx[offset + i] = current_vtx
                            break

        # 按大小排序每行的非零元素索引
        for row in range(self.rows):
            offset = self.row_offset[row]
            length = self.row_offset[row + 1] - offset
            self.elm_idx[offset : offset + length] = np.sort(
                self.elm_idx[offset : offset + length]
            )

    def mvp(self, x, y):
        """
        稀疏矩阵乘向量 (Matrix-Vector Product)
        """
        if len(x) != self.cols or len(y) != self.rows:
            raise ValueError(
                "Size mismatch: The number of columns in the matrix does not match the size of the vector."
            )

        y.fill(0.0)  # 初始化结果向量

        for r in range(self.rows):
            offset = self.row_offset[r]
            length = self.row_offset[r + 1] - offset

            local_sum = 0.0
            for i in range(length):
                col = self.elm_idx[offset + i]
                local_sum += self.elements[offset + i] * x[col]

            y[r] += local_sum


from collections import defaultdict


def get_Mloc_value(i, j, Mloc):
    """
    获取局部质量矩阵中的值
    """
    return Mloc[0] if i == j else Mloc[1]


def build_vertex_to_local_index(a, b, c):
    """
    构建顶点到局部索引的映射
    """
    return {a: 0, b: 1, c: 2}


def process_mass_matrix_row(M, current_row, i, vertex_to_local_index, Mloc):
    """
    处理质量矩阵中的一行
    """
    offset = M.row_offset[current_row]
    length = M.row_offset[current_row + 1] - offset

    for idx in range(length):
        col = M.elm_idx[offset + idx]

        # 检查列是否在当前三角形的顶点中
        if col in vertex_to_local_index:
            j = vertex_to_local_index[col]
            value = get_Mloc_value(i, j, Mloc)

            # 原子加法，避免并行冲突
            M.elements[offset + idx] += value


def build_mass_matrix(M):
    """
    构建全局质量矩阵
    """
    mesh = M.mesh

    for t in range(mesh.triangle_count()):
        # 获取三角形顶点
        a, b, c = mesh.indices[3 * t : 3 * t + 3]

        # 获取顶点坐标
        A = mesh.vertices[a]
        B = mesh.vertices[b]
        C = mesh.vertices[c]

        # 计算 AB 和 AC
        AB = B - A
        AC = C - A

        # 计算局部质量矩阵
        Mloc = mass_loc(AB, AC)

        # 构建顶点到局部索引的映射
        vertex_to_local_index = build_vertex_to_local_index(a, b, c)

        # 遍历三角形的每一个顶点（对应全局矩阵的行）
        for i, current_row in enumerate([a, b, c]):
            process_mass_matrix_row(M, current_row, i, vertex_to_local_index, Mloc)


def mass_loc(AB, AC):
    """
    计算局部质量矩阵的两个值
    """
    cross_product = np.cross(AB, AC)
    S_ABC = 0.5 * np.linalg.norm(cross_product)
    return S_ABC / 6.0, S_ABC / 12.0
