import numpy as np
import matplotlib.pyplot as plt
import sys
import os

# --- Configuration ---
INPUT_FILE = '../Input/radioactive_matrix.csv'  
OUTPUT_FILE = '../Output/out_radioactive_matrix.csv' 
DELIMITER = ','                  

# --- Visualization Display Settings ---
# SET to 1000 to visualize a center 1000x1000 sub-matrix
TARGET_SIZE = 2000 
VISUAL_ROWS = TARGET_SIZE
VISUAL_COLS = TARGET_SIZE 

# ------------------------------------

def get_center_submatrix(matrix, target_rows, target_cols):
    """
    Extracts a center sub-matrix from the original matrix.
    """
    if target_rows <= 0 or target_cols <= 0:
        raise ValueError("Visualization row and column counts must be greater than zero.")

    total_rows, total_cols = matrix.shape

    R = min(target_rows, total_rows)
    C = min(target_cols, total_cols)
    
    start_row = (total_rows - R) // 2
    end_row = start_row + R
    start_col = (total_cols - C) // 2
    end_col = start_col + C

    submatrix = matrix[start_row:end_row, start_col:end_col]
    
    # index_range now correctly holds the starting index (r_start, c_start)
    index_range = (start_row, end_row - 1, start_col, end_col - 1)
    
    return submatrix, index_range

# --- Main Visualization Function (FIXED FOR SUB-MATRIX AXIS SCALE) ---

def visualize_matrices(input_file, output_file, visual_rows, visual_cols, delimiter=','):
    
    data = {}
    print(f"Attempting to load data...")

    # Load Input Data 
    try:
        data['Input_Raw'] = np.genfromtxt(input_file, delimiter=delimiter, dtype=float)
        if data['Input_Raw'].size == 0:
            print(f"❌ Input file {input_file} is empty.")
            return
        if data['Input_Raw'].ndim == 1:
            data['Input_Raw'] = np.atleast_2d(data['Input_Raw'])
        print(f"✅ Loaded initial map (Shape: {data['Input_Raw'].shape})")
    except Exception as e:
        print(f"❌ Error loading input data: {e}")
        return

    # Load Output Data
    try:
        data['Output_Raw'] = np.genfromtxt(output_file, delimiter=delimiter, dtype=float)
        if data['Output_Raw'].size == 0:
            print(f"❌ Output file {output_file} is empty.")
            return
        if data['Output_Raw'].ndim == 1:
            data['Output_Raw'] = np.atleast_2d(data['Output_Raw'])
        print(f"✅ Loaded final map (Shape: {data['Output_Raw'].shape})")
    except Exception as e:
        print(f"❌ Error loading output data: {e}")
        return
        
    # 3. Extract and Prepare Data for Plotting
    try:
        data['Input'], (r_start, r_end, c_start, c_end) = \
            get_center_submatrix(data['Input_Raw'], visual_rows, visual_cols)
        
        data['Output'], _ = \
            get_center_submatrix(data['Output_Raw'], visual_rows, visual_cols)

        rows, cols = data['Input'].shape
        # Subtitle dynamically reflects sub-matrix size
        subtitle = f'Radioactive Contamination Simulation: Initial vs. Final Concentration (Center {rows}x{cols})'
        
    except ValueError as e:
        print(f"❌ Error in center submatrix calculation: {e}")
        return

    # --- FIX: Calculate INDEPENDENT color ranges ---
    
    v_min_input = data['Input'].min()
    v_max_input = data['Input'].max()
    v_min_output = data['Output'].min()
    v_max_output = data['Output'].max()
    
    # --- FIXED AXIS LABELING LOGIC ---
    
    # 1. Calculate the indices where ticks should appear on the plot (relative: 0 to rows/cols)
    tick_step_index = max(1, rows // 5) 
    index_ticks = np.arange(0, rows, tick_step_index)
    
    # 2. Determine the physical distance (in km) of the start of the sub-matrix
    # Scale factor: 100 meters/cell / 1000 meters/km = 0.1
    scale_factor_km = 100 / 1000.0 
    
    start_dist_km = r_start * scale_factor_km
    dist_step_km = tick_step_index * scale_factor_km
    
    # 3. Calculate the new labels in kilometers
    distance_ticks_km = [int(start_dist_km + i * dist_step_km) for i in np.arange(len(index_ticks))]

    # 4. Create and configure the plot
    fig, axes = plt.subplots(1, 2, figsize=(16, 8)) 

    # --- Plot 1: Initial Concentration Map ---
    im0 = axes[0].imshow(data['Input'], cmap='plasma', interpolation='nearest', 
                         vmin=v_min_input, vmax=v_max_input, aspect='auto')
    
    # Set plot-relative positions (index_ticks) and physical labels (distance_ticks_km)
    axes[0].set_xticks(index_ticks)
    axes[0].set_xticklabels(distance_ticks_km)
    axes[0].set_yticks(index_ticks)
    axes[0].set_yticklabels(distance_ticks_km)
    
    axes[0].set_title(f'Initial Concentration Map')
    axes[0].set_xlabel('East-West Distance (km)')
    axes[0].set_ylabel('South-North Distance (km)')
    
    # Add a separate color bar for the input data
    cbar_ax0 = fig.add_axes([0.02, 0.15, 0.03, 0.7]) 
    fig.colorbar(im0, cax=cbar_ax0).set_label('Initial Concentration C(x,y)', rotation=270, labelpad=15)
    
    # --- Plot 2: Final Concentration Map ---
    im1 = axes[1].imshow(data['Output'], cmap='plasma', interpolation='nearest', 
                         vmin=v_min_output, vmax=v_max_output, aspect='auto')

    # Set plot-relative positions (index_ticks) and physical labels (distance_ticks_km)
    axes[1].set_xticks(index_ticks)
    axes[1].set_xticklabels(distance_ticks_km)
    axes[1].set_yticks(index_ticks)
    axes[1].set_yticklabels(distance_ticks_km)

    axes[1].set_title(f'Final Concentration Map (After 100 iterations)')
    axes[1].set_xlabel('East-West Distance (km)')
    axes[1].set_ylabel('South-North Distance (km)')

    # 5. Add a separate color bar for the output data
    cbar_ax1 = fig.add_axes([0.94, 0.15, 0.03, 0.7]) 
    fig.colorbar(im1, cax=cbar_ax1).set_label('Final Concentration C(x,y)', rotation=270, labelpad=15)

    fig.suptitle(subtitle, fontsize=18)
    
    print("🚀 Displaying visualization. Close the window to exit the script.")
    plt.show()

# ----------------------------------------------------------------------
# --- Execution Block ---
# ----------------------------------------------------------------------

if __name__ == '__main__':
    try:
        if not os.path.isdir('../Input') or not os.path.isdir('../Output'):
            print("❌ Project structure missing: Please ensure 'Input' and 'Output' directories exist in the project root.")
            sys.exit(1)

        # Main call
        visualize_matrices(INPUT_FILE, OUTPUT_FILE, VISUAL_ROWS, VISUAL_COLS)

    except KeyboardInterrupt:
        print("\n\n🛑 Keyboard interrupt received. Closing visualization and exiting program.")
        plt.close('all') 
        sys.exit(0)
    except Exception as e:
        print(f"An unexpected error occurred: {e}")
        sys.exit(1)