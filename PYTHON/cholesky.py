import numpy as np

# 定义矩阵 A 和向量 b
A = np.array([
    [4, 12, -16],
    [12, 37, -43],
    [-16, -43, 98]
])
b = np.array([1, 2, 3])

L =np.linalg.cholesky(A)

# 解方程 Ax = b
x = np.linalg.solve(L.T, b)
print(L)
print(x)

