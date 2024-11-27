import numpy as np

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
        self.elements = np.zeros(total_nonzeros)    # 非零元素数组
        self.elm_idx = -np.ones(total_nonzeros, dtype=int)  # 列索引数组，初始化为 -1

        # 填充非零元素索引
        for t in range(mesh.triangle_count()):
            a, b, c = mesh.indices[3 * t:3 * t + 3]
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
            self.elm_idx[offset:offset + length] = np.sort(self.elm_idx[offset:offset + length])

    def mvp(self, x, y):
        """
        稀疏矩阵乘向量 (Matrix-Vector Product)
        """
        if len(x) != self.cols or len(y) != self.rows:
            raise ValueError("Size mismatch: The number of columns in the matrix does not match the size of the vector.")

        y.fill(0.0)  # 初始化结果向量

        for r in range(self.rows):
            offset = self.row_offset[r]
            length = self.row_offset[r + 1] - offset

            local_sum = 0.0
            for i in range(length):
                col = self.elm_idx[offset + i]
                local_sum += self.elements[offset + i] * x[col]

            y[r] += local_sum
