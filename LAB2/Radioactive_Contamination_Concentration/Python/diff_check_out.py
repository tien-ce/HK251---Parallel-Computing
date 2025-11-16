import pandas as pd
import numpy as np
import sys

# --- CONFIGURATION (CẤU HÌNH) ---
FILE_A = '../Output/out_radioactive_matrix_seq.csv'  # First input file (e.g., V1 Output)
FILE_B = '../Output/out_radioactive_matrix_paral.csv'  # Second input file (e.g., V2 Output)
DELIMITER = ','                     # Field delimiter (e.g., ',' or ';')
# Tolerance for floating-point comparison (e.g., 1e-6 means 0.000001)
FLOAT_TOLERANCE = 0.1              

def compare_matrices(file_a, file_b, delimiter=',', tolerance=1e-6):
    """
    Loads two CSV files and compares their data content position-by-position
    without relying on a Key Column. Suitable for comparing matrices.
    """
    print(f"🔬 Starting positional comparison: {file_a} and {file_b}")
    print(f"🌡️ Tolerance for floating-point comparison: {tolerance}\n")

    # 1. Load Data
    try:
        # Load without a header if necessary, or let pandas infer
        df_a = pd.read_csv(file_a, delimiter=delimiter, header=None)
        df_b = pd.read_csv(file_b, delimiter=delimiter, header=None)
        
        # NOTE: Using 'header=None' forces positional comparison regardless of column names.
        
    except FileNotFoundError as e:
        print(f"❌ Error: File not found - {e.filename}")
        return
    except Exception as e:
        print(f"❌ Error reading file: {e}")
        return

    # --- 2. Check Dimensions ---
    print("--- 2. Checking Dimensions ---")
    
    if df_a.shape != df_b.shape:
        print(f"🛑 Error: Matrix dimensions MUST match for positional comparison.")
        print(f"   - {file_a} has {df_a.shape[0]} rows, {df_a.shape[1]} columns.")
        print(f"   - {file_b} has {df_b.shape[0]} rows, {df_b.shape[1]} columns.")
        return
    else:
        rows, cols = df_a.shape
        print(f"✅ Dimensions match: {rows} rows x {cols} columns.")

    # --- 3. Positional Data Comparison ---
    print("\n--- 3. Analyzing Data Content (Positional) ---")

    # So sánh giá trị tuyệt đối giữa hai DataFrame
    # df_a và df_b phải là kiểu số (numeric)
    if not all(pd.api.types.is_numeric_dtype(df_a[c]) for c in df_a.columns):
        print("⚠️ Warning: DataFrames contain non-numeric types. Attempting conversion...")
        df_a = df_a.apply(pd.to_numeric, errors='coerce')
        df_b = df_b.apply(pd.to_numeric, errors='coerce')


    # Tính toán sự khác biệt tuyệt đối
    difference = (df_a - df_b).abs()
    
    # Xác định các ô có sự khác biệt lớn hơn tolerance
    # Nếu sự khác biệt nhỏ hơn hoặc bằng tolerance, coi là khớp
    mismatches = difference > tolerance
    
    # Đếm tổng số ô không khớp (Total Mismatches)
    total_mismatches = mismatches.sum().sum()
    
    # --- 4. Reporting Results ---
    
    if total_mismatches == 0:
        print("🎉 Excellent! All values match positionally (within tolerance).")
    else:
        print(f"🛑 Found {total_mismatches} cells with differences > {tolerance}.")
        
        # Tìm chi tiết các vị trí và giá trị khác biệt
        mismatch_locations = []
        for r in range(rows):
            for c in range(cols):
                if mismatches.iloc[r, c]:
                    mismatch_locations.append({
                        'Row': r,
                        'Col': c,
                        'Value A': df_a.iloc[r, c],
                        'Value B': df_b.iloc[r, c],
                        'Diff': difference.iloc[r, c]
                    })
                    if len(mismatch_locations) >= 5:
                        break
            if len(mismatch_locations) >= 5:
                break
        
        print("\nFirst 5 detailed mismatches found:")
        for item in mismatch_locations:
            print(f"   - Pos [{item['Row']}, {item['Col']}] | Diff: {item['Diff']:.8f} | A: {item['Value A']:.6f} | B: {item['Value B']:.6f}")

# --- RUN SCRIPT ---
if __name__ == '__main__':
    
    print("--- Running Matrix Comparison ---")
    
    # Bạn có thể thay đổi các tham số nếu cần
    # Ví dụ: Giả sử bạn có 2 file heat_matrix.csv và output.csv
    # Để kiểm tra, bạn cần đảm bảo các file này tồn tại
    # Thay thế tên file thật của bạn vào đây:
    
    compare_matrices(FILE_A, FILE_B, delimiter=DELIMITER, tolerance=FLOAT_TOLERANCE)
    
    print("---------------------------------")