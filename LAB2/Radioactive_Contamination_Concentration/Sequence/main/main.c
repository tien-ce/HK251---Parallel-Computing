#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>
#include "main.h"   // Define variable
#include "read_write_matrix.h" // Read and write matrix
#include "pde.h"    // PDE formulas
int main(int argc, char **argv){
    if (rows <=0 || cols <=0 || iterations <=0){
        perror (" Invalid arguments, all arguments must be positive integers");
        return -1;
    }
    
    // 1. ALLOCATE MEMORY for the temporary 'New' matrix and read matrix from input file
    float *matrix = (float*) malloc (rows * cols * sizeof(float));
    float *new_matrix = (float*) malloc (rows * cols * sizeof(float));
    // Read the matrix from the file
    int result = read_matrix1D (in_path, &rows, &cols, matrix);
    if (result != 0) {
        fprintf (stderr, "Failed to read matrix from file %s\n", in_path);
        // Free the allocated memory in case of failure
        free (matrix);
        return -1;
    }
    printf ("Matrix read successfully from file %s with dimensions %dx%d, interation %d\n", in_path, rows, cols, iterations);
    // Timer
    float start_time = MPI_Wtime();
    
    // 2. MAIN LOOP ( iterations)
    for (int t = 0; t < iterations; t++) {
        
        // 3. SPATIAL LOOP: Calculate new values for every internal cell
        for (int i = 0; i < rows; i ++){
            for (int j = 0; j < cols; j++){
                // Get needed value
                float c_i_j = matrix[i * cols + j];
                float c_im1_j = i - 1 >= 0 ? matrix[(i-1) * cols + j] :   0;
                float c_i_jm1 = j - 1 >= 0  ? matrix[i * cols + (j-1)] : 0;
                float c_ip1_j = i + 1 < rows ? matrix[(i + 1) * cols + j] : 0;
                float c_i_jp1 = j + 1 < cols ? matrix[i * cols + (j + 1)] : 0;
                // Step 1: Caculate the advection 
                float advec = calculate_advection (c_i_j, c_i_jm1, c_im1_j, ux, uy, dx, dy);

                // Step 2: Caculate the diffusion
                float diffu = calculate_diffusion (c_i_j, c_ip1_j, c_im1_j, c_i_jp1, c_i_jm1, D, dx, dy);

                // Step 3: Caculate the decay
                float decay = calculate_decay (c_i_j, lamda, k);

                // Step 4: Caculate C new
                new_matrix[i * cols + j] = update_concentration (c_i_j, (advec + diffu + decay), dt);
                if (new_matrix[i * cols + j] < 0.0 ){
                    fprintf (stderr, "Return error value from pde function\n");
                }
            }
        }
        
        // 4. SWAP: After calculating ALL new values, the new matrix becomes the old matrix for the next iteration
        float *temp = matrix;
        matrix = new_matrix;
        new_matrix = temp;
    }
    // Endtime 
    float end_time = MPI_Wtime();
    printf ("Sequential simulation finished in %.4f seconds\n", end_time - start_time);   
    
    // 5. Write result to output file and free allocated memory
    write_matrix1D (out_path, rows, cols, matrix);
    free (matrix);
    free (new_matrix);
    return 0;
}