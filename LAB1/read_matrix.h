#include <stdio.h>
#ifndef READ_MATRIX_H
#define READ_MATRIX_H
int read_matrix(const char *filename, int *rows, int *cols, float ***matrix);
int write_matrix(const char *filename, int rows, int cols, float ***matrix);
#endif