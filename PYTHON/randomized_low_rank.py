import numpy as np


def randomized_low_rank_approximation(A, k):
    """
    对矩阵 A 进行随机低秩近似，目标秩为 k
    """
    # Step 1: 随机投影
    m, n = A.shape
    P = np.random.randn(n, k)  # 随机矩阵
    Y = np.dot(A, P)  # 投影到子空间

    # Step 2: 正交化
    Q, _ = np.linalg.qr(Y)  # 对 Y 进行 QR 分解，Q 为正交基

    # Step 3: 在低维空间中进行计算
    B = np.dot(Q.T, A)  # 低维矩阵

    # Step 4: 构建低秩近似
    A_approx = np.dot(Q, B)

    return A_approx


# 测试
np.random.seed(42)
A = np.random.rand(500, 300)  # 原始矩阵
k = 100  # 目标秩
A_approx = randomized_low_rank_approximation(A, k)

print("Original shape:", A.shape)
print("Approximation shape:", A_approx.shape)
print("Approximation error (Frobenius norm):", np.linalg.norm(A - A_approx, "fro"))
