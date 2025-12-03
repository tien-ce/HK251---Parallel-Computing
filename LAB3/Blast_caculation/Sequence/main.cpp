#include <stdio.h>
#include <stdlib.h>
#include <ctime>
#include "main.h"
#include "read_write_matrix.h"
#include "blast_caculator.h"    

int main (int argc, char* argv[]){
    // 1. ALLOCATE MEMORY for the temporary 'New' matrix and read matrix from input file
    float *matrix = (float*) malloc (rows * cols * sizeof(float));
    float *new_matrix = (float*) malloc (rows * cols * sizeof(float));
    // Read the matrix from the file
    int result = read_matrix1D (in_path, &rows, &cols, matrix);
    if (result != 0) {
        fprintf (stderr, "Failed to read matrix from file %s\n", in_path);
        // Free the allocated memory in case of failure
        free (matrix);
        free (new_matrix);
        return -1;
    }
    printf ("Matrix read successfully from file %s with dimensions %dx%d, interation %d\n", in_path, rows, cols, iterations);
    
    // Timer
    time_t start_time = time(0);
    
    // 2. MAIN LOOP ( iterations)
    for (int t = 0; t < iterations; t++){
        // Traverse the array
        for (int i = 0; i < rows; i++){
            for (int j = 0; j < cols; j++){
                // 3. Calculate the SHOCK WAVE BLAST
                float R = calculate_R (i,j);    // The distance to center
                float arrival_time = calculate_t (R);
                if (arrival_time <= t) {
                    // Calculate Z and U only if the cell needs updating
                    float Z = calculate_z (R);
                    float U = calculate_u (Z);
                    
                    // Calculate Pso using the user-defined signature
                    float P = calculate_pso (U); 
                    
                    // Update output matrix with the Pso value
                    new_matrix[i * cols + j] = P;
                }
            }
        }
    }
    time_t end_time = time(0);
    printf ("Sequential simulation finished in %ld seconds\n", end_time - start_time);
        // 5. Write result to output file and free allocated memory
    write_matrix1D (out_path, rows, cols, new_matrix);
    free (matrix);
    free (new_matrix);
    return 0;
}