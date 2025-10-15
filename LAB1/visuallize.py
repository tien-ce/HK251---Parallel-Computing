import numpy as np
import matplotlib.pyplot as plt
import sys
# --- Configuration (Giữ nguyên) ---
INPUT_FILE = 'input/heat_matrix.csv'  
OUTPUT_FILE = 'output/output.csv'      
DELIMITER = ','                 
# ... (Phần code chính không thay đổi)
# ------------------------------------

def get_center_submatrix(matrix, target_rows, target_cols):
    """
    Trích xuất ma trận con từ tâm của ma trận gốc.

    Tham số:
    - matrix (np.ndarray): Ma trận gốc.
    - target_rows (int): Số hàng mong muốn của ma trận con (R).
    - target_cols (int): Số cột mong muốn của ma trận con (C).

    Trả về:
    - np.ndarray: Ma trận con đã được trích xuất.
    """
    if target_rows <= 0 or target_cols <= 0:
        raise ValueError("Số hàng và số cột trực quan hóa phải lớn hơn 0.")

    total_rows, total_cols = matrix.shape

    # 1. Xác định số hàng và cột thực tế (cắt bớt nếu lớn hơn ma trận gốc)
    R = min(target_rows, total_rows)
    C = min(target_cols, total_cols)

    # 2. Tính toán điểm bắt đầu (start_row, start_col)
    # Vị trí bắt đầu = (Tổng số - Kích thước mong muốn) // 2
    
    # Hàng
    start_row = (total_rows - R) // 2
    end_row = start_row + R
    
    # Cột
    start_col = (total_cols - C) // 2
    end_col = start_col + C

    # Trích xuất ma trận con
    submatrix = matrix[start_row:end_row, start_col:end_col]
    
    # Ghi nhận các chỉ số gốc để hiển thị trên biểu đồ
    # Đây là các chỉ số *bắt đầu* và *kết thúc* của khu vực được hiển thị
    index_range = (start_row, end_row - 1, start_col, end_col - 1)
    
    return submatrix, index_range

# --- Hàm trực quan hóa đã được sửa đổi ---

def visualize_two_matrices(input_file, output_file, visual_rows=None, visual_cols=None, delimiter=','):
    """
    Đọc hai file ma trận, trực quan hóa chúng cạnh nhau dưới dạng bản đồ nhiệt (heatmap),
    với khả năng chỉ hiển thị ma trận con trích xuất từ tâm.

    Tham số:
    - visual_rows (int, optional): Số hàng của ma trận con từ tâm muốn hiển thị.
    - visual_cols (int, optional): Số cột của ma trận con từ tâm muốn hiển thị.
    """
    
    data = {}
    
    # 1. Load data for both files
    # (Phần này giữ nguyên, nhưng tôi sẽ đưa logic trích xuất ma trận con vào sau khi load)
    try:
        data['Input_Raw'] = np.loadtxt(input_file, delimiter=delimiter)
        print(f"✅ Loaded input matrix from {input_file}")
    except FileNotFoundError:
        print(f"❌ Error: Input file not found at {input_file}")
        return
    except Exception as e:
        print(f"❌ Error loading input data: {e}")
        return

    try:
        data['Output_Raw'] = np.loadtxt(output_file, delimiter=delimiter)
        print(f"✅ Loaded output matrix from {output_file}")
    except FileNotFoundError:
        print(f"❌ Error: Output file not found at {output_file}")
        return
    except Exception as e:
        print(f"❌ Error loading output data: {e}")
        return
        
    # **2. Trích xuất ma trận con (nếu yêu cầu)**
    if visual_rows is not None and visual_cols is not None:
        try:
            # Xử lý ma trận Input
            data['Input'], (r_start, r_end, c_start, c_end) = \
                get_center_submatrix(data['Input_Raw'], visual_rows, visual_cols)
            
            # Xử lý ma trận Output
            data['Output'], _ = \
                get_center_submatrix(data['Output_Raw'], visual_rows, visual_cols)
            
            print(f"✨ Visualizing a sub-matrix of size {data['Input'].shape} from center.")
            print(f"   (Original Indices: Rows {r_start} to {r_end}, Cols {c_start} to {c_end})")
            
            # Cập nhật tiêu đề biểu đồ để phản ánh việc trích xuất
            subtitle = f'Heat Simulation: Input vs. Final Output (Center Sub-matrix {data["Input"].shape})'
            
        except ValueError as e:
            print(f"❌ Error in center submatrix calculation: {e}")
            return
    else:
        # Sử dụng toàn bộ ma trận nếu không có tham số trích xuất
        data['Input'] = data['Input_Raw']
        data['Output'] = data['Output_Raw']
        subtitle = 'Heat Simulation: Input vs. Final Output (Full Matrix)'
        r_start, c_start = 0, 0 # Chỉ số bắt đầu là 0
        
    # Xác định phạm vi màu chung
    all_values = np.concatenate([data['Input'].flatten(), data['Output'].flatten()])
    v_min = all_values.min()
    v_max = all_values.max()
    
    # 3. Create and configure the figure
    fig, axes = plt.subplots(1, 2, figsize=(14, 6))

    # 4. Plot the Input Matrix (Left Subplot)
    im0 = axes[0].imshow(data['Input'], cmap='hot', interpolation='nearest', 
                         vmin=v_min, vmax=v_max, aspect='auto')
    
    # **Điều chỉnh nhãn trục để hiển thị chỉ số gốc**
    rows, cols = data['Input'].shape
    axes[0].set_xticks(np.arange(0, cols, max(1, cols // 5))) # Hiển thị 5-6 ticks
    axes[0].set_xticklabels(np.arange(c_start, c_start + cols, max(1, cols // 5)))
    axes[0].set_yticks(np.arange(0, rows, max(1, rows // 5)))
    axes[0].set_yticklabels(np.arange(r_start, r_start + rows, max(1, rows // 5)))
    
    axes[0].set_title(f'Initial Heat Map (Input)')
    axes[0].set_xlabel(f'Column Index (Original: {c_start} to {c_start + cols - 1})')
    axes[0].set_ylabel(f'Row Index (Original: {r_start} to {r_start + rows - 1})')
    
    # 5. Plot the Output Matrix (Right Subplot)
    im1 = axes[1].imshow(data['Output'], cmap='hot', interpolation='nearest', 
                         vmin=v_min, vmax=v_max, aspect='auto')

    # **Điều chỉnh nhãn trục cho ma trận Output**
    axes[1].set_xticks(np.arange(0, cols, max(1, cols // 5)))
    axes[1].set_xticklabels(np.arange(c_start, c_start + cols, max(1, cols // 5)))
    axes[1].set_yticks(np.arange(0, rows, max(1, rows // 5)))
    axes[1].set_yticklabels(np.arange(r_start, r_start + rows, max(1, rows // 5)))

    axes[1].set_title(f'Final Heat Map (Output after simulation)')
    axes[1].set_xlabel(f'Column Index (Original: {c_start} to {c_start + cols - 1})')
    axes[1].set_ylabel(f'Row Index (Original: {r_start} to {r_start + rows - 1})')

    # 6. Add a single color bar for both plots
    fig.subplots_adjust(right=0.85)
    cbar_ax = fig.add_axes([0.90, 0.15, 0.03, 0.7])
    fig.colorbar(im1, cax=cbar_ax).set_label('Temperature Value')

    fig.suptitle(subtitle, fontsize=16)
    
    # 7. Display the figure
    print("🚀 Displaying visualization. Press Ctrl+C to close the window and exit the script.")
    plt.show()

# ----------------------------------------------------------------------
# 🌟 KEY MODIFICATION: Add Signal Handling in the main block
# ----------------------------------------------------------------------

if __name__ == '__main__':
    # --- Configuration (Ví dụ) ---
    
    # Nếu bạn muốn trực quan hóa ma trận con 400x400 từ tâm
    TARGET_ROWS = 400
    TARGET_COLS = 400
    
    try:
        # Gọi hàm với tham số kích thước ma trận con
        # Trong trường hợp không có file, hàm sẽ báo lỗi FileNotFoundError
        # Để chạy thử nghiệm, bạn cần đảm bảo các file này tồn tại hoặc chạy hàm với tham số None
        
        # Ví dụ 1: Trực quan hóa ma trận con 400x400
        visualize_two_matrices(INPUT_FILE, OUTPUT_FILE, visual_rows=TARGET_ROWS, visual_cols=TARGET_COLS)

        # Ví dụ 2: Trực quan hóa toàn bộ ma trận (nếu file tồn tại)
        # visualize_two_matrices(INPUT_FILE, OUTPUT_FILE)

        # # Ví dụ 3: Giả lập ma trận và chạy để kiểm tra logic trích xuất
        # print("\n--- Testing Sub-matrix Logic ---")
        # # Tạo ma trận giả 10x10
        # mock_input = np.arange(100).reshape(10, 10) 
        # # Trích xuất 4x6 (hàng x cột) từ tâm
        # sub_matrix, index_range = get_center_submatrix(mock_input, 4, 6)
        # print(f"Original 10x10 matrix:\n{mock_input}")
        # print(f"\nSub-matrix 4x6 from center (Rows {index_range[0]} to {index_range[1]}, Cols {index_range[2]} to {index_range[3]}):\n{sub_matrix}")
        
        # Nếu ma trận gốc (10x10) không tồn tại file, bạn có thể tạm thời gọi
        # visualize_two_matrices(INPUT_FILE, OUTPUT_FILE, visual_rows=400, visual_cols=400)
        # để kiểm tra logic lỗi file

    except KeyboardInterrupt:
        # This block catches the Ctrl+C signal
        print("\n\n🛑 Keyboard interrupt received. Closing visualization and exiting program.")
        plt.close('all') 
        sys.exit(0)
    except Exception as e:
        print(f"An unexpected error occurred: {e}")
        sys.exit(1)