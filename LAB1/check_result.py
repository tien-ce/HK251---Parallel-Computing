import numpy as np
from scipy.signal import convolve2d

# ======================= Cấu hình =======================
input_path  = "test.csv"      # giống file input của C
output_path = "output_python.csv"    # file xuất để so sánh
iterations  = 100                    # số vòng lặp
padding_val = 30                     # giống với C code

# Kernel 3x3 như trong sequence_convolution.c
kernel = np.array([
    [0.05, 0.1, 0.05],
    [0.1,  0.4, 0.1],
    [0.05, 0.1, 0.05]
], dtype=np.float32)

# ======================= Đọc ma trận =======================
#print(f"Reading input matrix from: {input_path}")
matrix = np.loadtxt(input_path, delimiter=",", dtype=np.float32)
rows, cols = matrix.shape
#print(f"Matrix size: {rows} x {cols}")

# ======================= Thực hiện convolution =======================
for it in range(iterations):
    #print (f"\nAfter iteration {it+1}:", end="\n")
    # mode='same' để giữ kích thước ma trận không đổi
    # boundary='fill', fillvalue=30 để padding bằng 30 giống C
    matrix = convolve2d(matrix, kernel, mode='same',
                        boundary='fill', fillvalue=padding_val)
# ======================= Ghi ma trận kết quả =======================
np.savetxt(output_path, matrix, fmt="%.2f", delimiter=",")
#print(f"Result saved to: {output_path}")

# ======================= (Tùy chọn) Tạo checksum để so sánh nhanh =======================
checksum = np.sum(matrix)
#print(f"Checksum (sum of all elements): {checksum:.6f}")
