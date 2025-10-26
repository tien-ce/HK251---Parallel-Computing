#include <stdio.h>
#include "read_matrix.h"
#include <stdlib.h>
#include <time.h>
#include <omp.h>
const int TILING_SIZE = 128;
const char *file_path = "input/heat_matrix.csv";
const char *out_path = "output/output_seq.csv";
const int iterations = 100;
const int num_threads = 4;
// Define the convolution kernel (3x3 averaging filter)
float kernel[3][3] = {
    {0.05, 0.1, 0.05},
    {0.1,  0.4, 0.1},
    {0.05, 0.1, 0.05}
};
void sequence_convolution(float *input, float *output, int rows, int cols) {
    // Padding size is 1 for 3x3 kernel
    for (int i = 0; i < rows; i++) {
        for (int j = 0 ; j < cols; j++) {
            float sum = 0.0;
            // Convolution operation
            for (int ki = -1; ki <=1 ; ki ++){
                for (int kj = -1; kj <= 1; kj ++){
                    // Padding with 30 if out of bounds
                    float tmp = (i +ki >=0 && j + kj >=0 && i + ki < rows && j + kj < cols) ? input[(i + ki) * cols + (j + kj)] : 30;
                    sum += tmp * kernel[ki + 1][kj + 1];
                }
            }
            output[i * cols + j] = sum;
        }
    }
}

void v1_parallel_convolution(float *input, float *output, int rows, int cols) {
    // To be implemented
    #pragma omp parallel for num_threads(num_threads) default (none) shared(input, output, kernel, rows, cols) schedule (dynamic)
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            float sum = 0.0;
            // Convolution operation
            for (int ki = -1; ki <= 1; ki++) {
                for (int kj = -1; kj <= 1; kj++) {
                    // Padding with 30 if out of bounds
                    float tmp = (i + ki >= 0 && j + kj >= 0 && i + ki < rows && j + kj < cols) ? input[(i + ki) * cols + (j + kj)] : 30;
                    sum += tmp * kernel[ki + 1][kj + 1];
                }
            }
            output[i * cols + j] = sum;
        }
    }
}

void v2_parallel_convolution(float *input, float *output, int rows, int cols) {
    #pragma omp parallel for num_threads(num_threads) default (none) shared(input, output, kernel, rows, cols, TILING_SIZE) schedule (dynamic)
    for (int i = 0; i < rows; i+= TILING_SIZE){
        for (int j = 0; j < cols; j+= TILING_SIZE){
            for (int ii = i; ii < i + TILING_SIZE && ii < rows; ii++) {
                for (int jj = j; jj < j + TILING_SIZE && jj < cols; jj++) {
                    float sum = 0.0;
                    // Convolution operation
                    for (int ki = -1; ki <= 1; ki++) {
                        for (int kj = -1; kj <= 1; kj++) {
                            // Padding with 30 if out of bounds
                            float tmp = (ii + ki >= 0 && jj + kj >= 0 && ii + ki < rows && jj + kj < cols) ? input[(ii + ki) * cols + (jj + kj)] : 30;
                            sum += tmp * kernel[ki + 1][kj + 1];
                        }
                    }
                    output[ii * cols + jj] = sum;
                }
            }
        }
    }    
}

int main(int argc, char **argv){
    if (argc <= 3){
        perror (" At least 3 arguments needed <rows> <cols> <iterations>");
        return -1;
    }
    const char *filename = file_path;
    int rows = atoi(argv[1]), cols = atoi(argv[2]), iterations = atoi(argv[3]);
    if (rows <=0 || cols <=0 || iterations <=0){
        perror (" Invalid arguments, all arguments must be positive integers");
        return -1;
    }
    if (argc >= 5){
        printf ("Output file is %s\n", argv[4]);
        out_path = argv[4];
    }
    if (argc == 6){
        filename = argv[5];
        printf ("Input file is %s\n", filename);
    }
    // Allocate memory for the matrix
    // float **matrix = (float**) malloc (rows * sizeof(float*));
    // for (int i = 0; i < rows; i++) {
    //     matrix[i] = (float*) malloc (cols * sizeof(float));
    // }
    // Allocate memory for the matrix with 1D array
    // int result = read_matrix (filename, &rows, &cols, &matrix); // Read the matrix from the file

    float *matrix = (float*) malloc (rows * cols * sizeof(float));
    // Read the matrix from the file
    int result = read_matrix1D (filename, &rows, &cols, matrix);
    if (result != 0) {
        fprintf (stderr, "Failed to read matrix from file %s\n", filename);
        // Free the allocated memory in case of failure
        free (matrix);
        return -1;
    }
    printf ("Matrix read successfully from file %s with dimensions %dx%d, interation %d\n", filename, rows, cols, iterations);
    time_t start_time = time(NULL);    
    // Test print the matrix
    #ifdef DEBUG
        for (int i = 0; i < 10; i++) {
            printf ("Row %d: ", i);
            for (int j = 0; j < 10; j++) {
                printf ("%.2f ", matrix[i][j]);
            }
            printf ("\n");
        }
    #endif
    // Allocate memory for the output matrix
    float *output_matrix = (float*) malloc (rows * cols * sizeof(float));
    #ifdef SEQUENCE
        for (int i=0 ; i < iterations; i++){
            sequence_convolution (matrix, output_matrix, rows, cols);
            #ifdef DEBUG
                printf ("After iteration %d:\n", i + 1);
                for (int r = 0; r < 10; r++) {
                    printf ("Row %d: ", r);
                    for (int c = 0; c < 10; c++) {
                        printf ("%.2f ", output_matrix[r][c]);
                    }
                    printf ("\n");
                }
            #endif
            #ifdef VISUALLIZATION
                const char *signal_file = "iteration_ready.flag";
                // Create a signal file to indicate the completion of an iteration
                FILE *file = fopen(signal_file, "w");
                if (file != NULL) {
                    fprintf(file, "Iteration %d completed\n", i + 1);
                    fclose(file);
                    printf ("Signal file %s created.\n", signal_file);
                } else {
                    perror("Error creating signal file");
                }
                printf ("Iteration %d completed, signal file %s created.\n", i + 1, signal_file);   

            #endif    
            // Swap the input and output matrices for the next iteration
            float *temp = matrix;
            matrix = output_matrix;
            output_matrix = temp;
        }
    #endif


    #ifdef PARALLEL_V1
        printf ("Using parallel version 1 with %d threads\n", num_threads);
        for (int i=0 ; i < iterations; i++){
            v1_parallel_convolution (matrix, output_matrix, rows, cols);
            #ifdef DEBUG
                printf ("After iteration %d:\n", i + 1);
                for (int r = 0; r < 10; r++) {
                    printf ("Row %d: ", r);
                    for (int c = 0; c < 10; c++) {
                        printf ("%.2f ", output_matrix[r * cols + c]);
                    }
                    printf ("\n");
                }
            #endif
            // Swap the input and output matrices for the next iteration
            float *temp = matrix;
            matrix = output_matrix;
            output_matrix = temp;
        }
    #endif
    #ifdef PARALLEL_V2
    printf ("Using parallel version 2 with %d threads, TILING_SIZE = %d\n", num_threads, TILING_SIZE);
        for (int i=0 ; i < iterations; i++){
            v2_parallel_convolution (matrix, output_matrix, rows, cols);
            #ifdef DEBUG
                printf ("After iteration %d:\n", i + 1);
                for (int r = 0; r < 10; r++) {
                    printf ("Row %d: ", r);
                    for (int c = 0; c < 10; c++) {
                        printf ("%.2f ", output_matrix[r * cols + c]);
                    }
                    printf ("\n");
                }
            #endif
            // Swap the input and output matrices for the next iteration
            float *temp = matrix;
            matrix = output_matrix;
            output_matrix = temp;
        }
    #endif         
        time_t end_time = time(NULL);
        double elapsed_time = difftime(end_time, start_time);
        printf ("Total time for %d iterations: %.2f seconds\n", iterations, elapsed_time);
    #ifdef WRITEOUT
        printf ("Writing output matrix to file %s\n", out_path);
        write_matrix1D (out_path, rows, cols, matrix);
    #endif
    // Free the allocated memory
    free (matrix);
    free (output_matrix);
}