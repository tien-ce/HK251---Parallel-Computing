#include "read_matrix.h"
#include <stdlib.h>
#include <string.h>
/*
    * @brief: read a matrix from a csv file
    * @param filename: the name of the file to read
    * @param rows: pointer to the number of rows
    * @param cols: pointer to the number of columns
    * @param read_matrix: pointer to the matrix to read (allocated in main)
    * @return: 0 if successful, -1 if error
*/
int read_matrix (const char * filename, int *rows, int *cols, float ***read_matrix){
    FILE *file = fopen(filename, "r");
    if (file == NULL) {     // Check if file opened successfully
        perror ("Error opening file");
        return -1;
    }
    char* line = (char*) malloc (50000 * sizeof(char)); // Allocate memory for a line )
    if (line == NULL) {
        perror ("Error allocating memory");
        fclose(file);
        return -1;
    }
    int row_count = 0; // Track number of rows read
    // Read the file line by line until we reach the end of the file or the specified number of rows
    while (fgets (line, 50000, file) && row_count < *rows) {
        // Process the line
        char *tmp = strtok (line, ","); // tokenlize the first element by ","
        int col_count = 0; // Track number of columns read
        while (tmp !=NULL && col_count < *cols) {
            (*read_matrix)[row_count][col_count] = atof (tmp); // Convert string to float and store in matrix
            tmp = strtok (NULL, ","); // Get the next token
            col_count++;
        }
        row_count++;
    }
    free(line);
    fclose(file);
    return 0;   // Return success
}

/*
    * @brief: write a matrix to a csv file
    * @param filename: the name of the file to write
    * @param rows: the number of rows
    * @param cols: the number of columns
    * @param matrix: the matrix to write
    * @return: 0 if successful, -1 if error
*/
int write_matrix (const char *filename, int rows, int cols, float ***matrix){
    FILE *file = fopen (filename, "w");
    if (file == NULL) {     // Check if file opened successfully
        perror ("Error opening file");
        return -1;
    }
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            fprintf (file, "%.2f", (*matrix)[i][j]); // Write the element to the file
            if (j < cols - 1) {
                fprintf (file, ","); // Add a comma if not the last element in the row
            }
        }
        fprintf (file, "\n"); // New line at the end of each row
    }
    fclose(file);
    return 0;   // Return success
}
int read_matrix1D (const char * filename, int *rows, int *cols, float *matrix){
    FILE *file = fopen(filename, "r");
    if (file == NULL) {     // Check if file opened successfully
        perror ("Error opening file");
        return -1;
    }
    char* line = (char*) malloc (50000 * sizeof(char)); // Allocate memory for a line )
    if (line == NULL) {
        perror ("Error allocating memory");
        fclose(file);
        return -1;
    }
    int row_count = 0; // Track number of rows read
    // Read the file line by line until we reach the end of the file or the specified
    while (fgets (line, 50000, file) && row_count < *rows) {
        // Process the line
        char *tmp = strtok (line, ","); // tokenlize the first element by ","
        int col_count = 0; // Track number of columns read
        while (tmp !=NULL && col_count < *cols) {
            matrix[row_count * (*cols) + col_count] = atof (tmp); // Convert string to float and store in matrix
            tmp = strtok (NULL, ","); // Get the next token
            col_count++;
        }
        row_count++;
    }
    free(line);
    fclose(file);
    return 0;   // Return success
}
int write_matrix1D (const char *filename, int rows, int cols, float *matrix){
    FILE *file = fopen (filename, "w");
    if (file == NULL) {     // Check if file opened successfully
        perror ("Error opening file");
        return -1;
    }
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            fprintf (file, "%.2f", matrix[i * cols + j]); // Write the element to the file
            if (j < cols - 1) {
                fprintf (file, ","); // Add a comma if not the last element in the row
            }
        }
        fprintf (file, "\n"); // New line at the end of each row
    }
    fclose(file);
    return 0;   // Return success
}