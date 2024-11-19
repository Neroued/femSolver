import numpy as np
from scipy.linalg import lu

import numpy as np

def lu_decomposition(A):
    """
    实现LU分解：将矩阵 A 分解为 L 和 U，使得 A = LU
    A: 输入的 n x n 矩阵
    返回:
        L: 单位下三角矩阵
        U: 上三角矩阵
    """
    n = A.shape[0]
    L = np.zeros_like(A, dtype=float)
    U = np.zeros_like(A, dtype=float)

    for i in range(n):
        # 计算U的第i行
        for j in range(i, n):
            U[i, j] = A[i, j] - sum(L[i, k] * U[k, j] for k in range(i))
        
        # 计算L的第i列
        for j in range(i, n):
            if i == j:
                L[i, i] = 1  # 对角线元素为1
            else:
                L[j, i] = (A[j, i] - sum(L[j, k] * U[k, i] for k in range(i))) / U[i, i]

    return L, U

# 示例测试
A = np.array([[4, 0, 0, 7],
              [0, 3, 4, 0],
              [0, 4, 3, 0],
              [7, 0, 0, 6]], dtype=float)

L, U = lu_decomposition(A)
print("矩阵 A:")
print(A)
print("下三角矩阵 L:")
print(L)
print("上三角矩阵 U:")
print(U)

# 验证 A = LU
print("验证 A = LU:")
print(np.dot(L, U))