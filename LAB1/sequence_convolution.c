#include <stdio.h>
#include "read_matrix.h"
#include <stdlib.h>

const char *file_path = "heat_matrix.csv";
const char *out_path = "output.csv";
const int iterations = 100;
void sequence_convolution(float **input, float **output, int rows, int cols) {
    // Define the convolution kernel (3x3 averaging filter)
    float kernel[3][3] = {
        {0.05, 0.1, 0.05},
        {0.1,  0.4, 0.1},
        {0.05, 0.1, 0.05}
    };
    // Padding size is 1 for 3x3 kernel
    for (int i = 0; i < rows; i++) {
        for (int j = 0 ; j < cols; j++) {
            float sum = 0.0;
            // Convolution operation
            for (int ki = -1; ki <=1 ; ki ++){
                for (int kj = -1; kj <= 1; kj ++){
                    // Padding with 30 if out of bounds
                    int tmp = (i +ki >=0 && j + kj >=0 && i + ki < rows && j + kj < cols) ? input [i + ki][j + kj] : 30;
                    sum += tmp * kernel[ki + 1][kj + 1];
                }
            }
            output[i][j] = sum;
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
    float **matrix = (float**) malloc (rows * sizeof(float*));
    for (int i = 0; i < rows; i++) {
        matrix[i] = (float*) malloc (cols * sizeof(float));
    }
    int result = read_matrix (filename, &rows, &cols, &matrix); // Read the matrix from the file
    if (result != 0) {
        fprintf (stderr, "Failed to read matrix from file %s\n", filename);
        // Free the allocated memory in case of failure
        for (int i = 0; i < rows; i++) {
            free (matrix[i]);
        }
        free (matrix);
        return -1;
    }
    printf ("Matrix read successfully from file %s with dimensions %dx%d\n", filename, rows, cols);
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
    float **output_matrix = (float**) malloc (rows * sizeof(float*));
    for (int i = 0; i < rows; i++) {
        output_matrix[i] = (float*) malloc (cols * sizeof (float));
    }
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
            // Swap the input and output matrices for the next iteration
            float **temp = matrix;
            matrix = output_matrix;
            output_matrix = temp;
        }
    #endif
    write_matrix (out_path, rows, cols, &matrix);
    // Free the allocated memory
    for (int i = 0; i < rows; i++) {
        free (matrix [i]);
        free (output_matrix[i]);
    }
    free (matrix);
    free (output_matrix);
}