#include "work_pool.h"
#include "read_write_matrix.h"
#include "blast_caculator.h"
#include "main.h"
#include <stdio.h>
#include <ctime>

void blast_caculation (void* paramaters) {
    if (paramaters == nullptr){
        perror ("[ERROR]: Pass parameter with the value is NUL");
    }
    Blast_parameter* p = (Blast_parameter*) paramaters;
    int start_row = p->start_row;
    int end_row = p->end_row;
    int cols = p->cols;
    int current_time = p->current_sim_time;
    float* out_matrix = p->output_matrix;
    for (int i = start_row; i <= end_row; i ++){
        for (int j = 0 ; j < cols; j++){
            int index = i * cols + j;
            if (out_matrix[index] != 0.00)  continue;

            float R = calculate_R (i,j);
            float arrival_time = calculate_t(R);

            if (arrival_time <= current_time) {
                float Z = calculate_z (R);
                float U = calculate_u(Z);                
                float P = (float)calculate_pso(U); 
                out_matrix[index] = P;
            }
        }
    }
    if (paramaters != nullptr)  delete p;
}

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
    
    // Initialize worker
    std::vector <Worker*> workers;
    for (int i = 0; i < NUM_THREAD; i ++)   workers.push_back(new Worker(i));
    
    // Timer
    time_t start_time = time(0);
    
    // 2. MAIN LOOP ( iterations)
    for (int t = 1; t < iterations + 1; t++ ){
        int start_row = 0;
        int end_row = 0;
        int block = rows / NUM_THREAD;
        int id = 0;
        while (start_row <= end_row && id < NUM_THREAD){
            start_row = id * block;
            end_row = start_row + block - 1;
            if (end_row > rows)     end_row = rows;
            if (start_row > end_row)    break;
            id += 1;
            Blast_parameter* bl_p = new Blast_parameter(
                 start_row,
                 end_row,
                 cols, 
                 t, // T
                 new_matrix );
            Task* task = new Task (blast_caculation, (void*)bl_p);
            TaskQueue::get()->enqueue(task);
        }   
        TaskQueue::get()->wait_for_batch_completion();
    }
    time_t end_time = time(0);
    for (int i = 0; i < NUM_THREAD; i ++) workers[i]->exit();
    printf ("Parallel simulation finished in %ld seconds\n", end_time - start_time);
    write_matrix1D (out_path,rows,cols,new_matrix);
    free (matrix);
    free (new_matrix);
}