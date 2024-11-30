import numpy as np

# 定义矩阵 A 和向量 b
A = np.array(
    [
        [10, 2, 0, 0, 1],
        [2, 10, 3, 0, 0],
        [0, 3, 10, 4, 0],
        [0, 0, 4, 10, 5],
        [1, 0, 0, 5, 10],
    ]
)
b = np.array([1, 2, 3, 4, 5])

L = np.linalg.cholesky(A)

# 解方程 Ax = b
x = np.linalg.solve(A, b)
print(L)
print("x: ", x)
