import numpy as np
import matplotlib.pyplot as plt
import sys
import os

# --- Configuration ---
INPUT_FILE = '../Input/blast_matrix.csv'  
OUTPUT_FILE = '../Output/seq_blast_matrix.csv' 
DELIMITER = ','                  
MAP_SIZE = 4000 # The full dimension of the matrix
CELL_SIZE_METERS = 10 # 10 meters per cell

# --- Visualization Display Settings ---
# You are working on a 4000x4000 matrix. 
# Set TARGET_SIZE to 4000 to view the whole map.
# Set it lower (e.g., 200) to zoom into the center around the blast.
TARGET_SIZE = 1000 
VISUAL_ROWS = TARGET_SIZE
VISUAL_COLS = TARGET_SIZE 

# ------------------------------------

def get_center_submatrix(matrix, target_rows, target_cols):
    """
    Extracts a center sub-matrix from the original matrix.
    The indices returned are the starting and ending indices in the FULL matrix.
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
    
    # index_range: (start_row, end_row_index, start_col, end_col_index)
    index_range = (start_row, end_row - 1, start_col, end_col - 1)
    
    return submatrix, index_range

def visualize_matrices(input_file, output_file, visual_rows, visual_cols, delimiter=','):
    
    data = {}
    print(f"Attempting to load data...")

    # Load Input Data (Initial Map: Should be all zeros)
    try:
        data['Input_Raw'] = np.genfromtxt(input_file, delimiter=delimiter, dtype=float)
        if data['Input_Raw'].size == 0:
            print(f"❌ Input file {input_file} is empty.")
            return
        # Ensure it's treated as a 2D array even if it's only one row/column
        if data['Input_Raw'].ndim == 1:
            data['Input_Raw'] = np.atleast_2d(data['Input_Raw'])
        print(f"✅ Loaded initial map (Shape: {data['Input_Raw'].shape})")
    except Exception as e:
        print(f"❌ Error loading input data: {e}")
        return

    # Load Output Data (Final Map: Should contain Pso values)
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
        # Extract the center sub-matrix for visualization
        data['Input'], (r_start, r_end, c_start, c_end) = \
            get_center_submatrix(data['Input_Raw'], visual_rows, visual_cols)
        
        data['Output'], _ = \
            get_center_submatrix(data['Output_Raw'], visual_rows, visual_cols)

        rows, cols = data['Input'].shape
        subtitle = f'Blast Simulation: Initial vs. Final Overpressure (Center {rows}x{cols} cells)'
        
    except ValueError as e:
        print(f"❌ Error in center submatrix calculation: {e}")
        return

    # --- Axis and Color Bar Setup ---
    
    # Calculate the indices where ticks should appear on the plot (relative to the sub-matrix)
    # Use 5-10 ticks max for clarity
    tick_count = 5 
    tick_step_index = max(1, rows // tick_count) 
    index_ticks = np.arange(0, rows, tick_step_index)
    
    # Scale factor: 10 meters/cell / 1000 meters/km = 0.01 km/cell
    scale_factor_km = CELL_SIZE_METERS / 1000.0 
    
    # Calculate the physical distance (in km) of the labels
    start_dist_km = r_start * scale_factor_km
    dist_step_km = tick_step_index * scale_factor_km
    
    # Calculate the new labels in kilometers, rounding to the nearest integer for clean labels
    # Note: Labels show the distance from the map's original (0,0) corner.
    distance_labels_km = [round(start_dist_km + i * dist_step_km) for i in np.arange(len(index_ticks))]

    # Get the global min/max for color bar scaling (Pso is typically > 0)
    v_min_output = data['Output'].min()
    v_max_output = data['Output'].max()
    
    # Initialize plot
    fig, axes = plt.subplots(1, 2, figsize=(16, 8)) 
    
    # --- Plot 1: Initial Concentration Map (All Zeros) ---
    # Use fixed vmin/vmax for input since it should be uniform (0)
    im0 = axes[0].imshow(data['Input'], cmap='viridis', interpolation='nearest', 
                         vmin=0, vmax=v_max_output, aspect='auto')
    
    # Apply custom ticks
    axes[0].set_xticks(index_ticks)
    axes[0].set_xticklabels(distance_labels_km)
    axes[0].set_yticks(index_ticks)
    axes[0].set_yticklabels(distance_labels_km)
    
    axes[0].set_title(f'Initial State (T=0s)')
    axes[0].set_xlabel('Distance (km)')
    axes[0].set_ylabel('Distance (km)')
    
    # Add color bar for input (should only show 0)
    cbar_ax0 = fig.add_axes([0.02, 0.15, 0.03, 0.7]) 
    fig.colorbar(im0, cax=cbar_ax0).set_label('Peak Overpressure Pso (kPa)', rotation=270, labelpad=15)
    
    # --- Plot 2: Final Concentration Map (Simulated Result) ---
    im1 = axes[1].imshow(data['Output'], cmap='viridis', interpolation='nearest', 
                         vmin=v_min_output, vmax=v_max_output, aspect='auto')

    # Apply custom ticks
    axes[1].set_xticks(index_ticks)
    axes[1].set_xticklabels(distance_labels_km)
    axes[1].set_yticks(index_ticks)
    axes[1].set_yticklabels(distance_labels_km)

    axes[1].set_title(f'Final State (T=100s)')
    axes[1].set_xlabel('Distance (km)')
    axes[1].set_ylabel('Distance (km)')

    # Add color bar for output (showing the Pso range)
    cbar_ax1 = fig.add_axes([0.94, 0.15, 0.03, 0.7]) 
    fig.colorbar(im1, cax=cbar_ax1).set_label('Peak Overpressure Pso (kPa)', rotation=270, labelpad=15)

    fig.suptitle(subtitle, fontsize=18)
    
    print("Displaying visualization. Close the window to exit the script.")
    plt.show()

# ----------------------------------------------------------------------
# --- Execution Block ---
# ----------------------------------------------------------------------

if __name__ == '__main__':
    # Verify the necessary directories exist for the C++ code to write to
    if not os.path.exists('../Input'):
        os.makedirs('../Input')
    if not os.path.exists('../Output'):
        os.makedirs('../Output')

    # Check if the initial file exists (it should be created by init_matrix.cpp)
    if not os.path.exists(INPUT_FILE):
        print(f"Warning: Input file {INPUT_FILE} not found. Please compile and run init_matrix.cpp first.")
        # If the file doesn't exist, we cannot proceed with visualization setup.
    
    try:
        # Main visualization call
        visualize_matrices(INPUT_FILE, OUTPUT_FILE, VISUAL_ROWS, VISUAL_COLS)

    except Exception as e:
        print(f"An unexpected error occurred during visualization: {e}")
        sys.exit(1)