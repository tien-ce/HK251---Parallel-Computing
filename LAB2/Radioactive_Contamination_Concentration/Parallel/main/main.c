#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>
#include "main.h"   // Define variable
#include "read_write_matrix.h" // Read and write matrix
#include "pde.h"    // PDE formulas
int main(int argc, char **argv){
    // 1. INITIALIZE MPI for 4 processes
    int finished = 0;    
    MPI_Init (&argc, &argv);
    int world_rank = -1, world_size = 0;
    MPI_Comm_rank (MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size (MPI_COMM_WORLD, &world_size);
    if (world_size < 2){
        if (world_rank == 0)    
            fprintf (stderr, "At least 2 processes");
        
        MPI_Abort (MPI_COMM_WORLD, MPI_ERR_ARG);
    }
    if (argc > 1){
        iterations = atoi (argv[1]);
        if (world_rank == root_rank)    
            printf ("Receive iteration %d from argument\n", iterations);
    }
    if (rows <=0 || cols <=0 || iterations <=0){
        perror (" Invalid arguments, all arguments must be positive integers");
        return -1;
    }
    int chunk = (rows + world_size -1 ) / world_size; // cell (row / world_size)    
    // 2. ALLOCATE MEMORY for the root process to read data from input file
    float *matrix;
    if (world_rank == root_rank){
    // Just allocate for root process   
        matrix = (float*) malloc (chunk * world_size * cols * sizeof(float));   // We be redudant if rows is not divisible by world_size.
        // Read the matrix from the file
        int result = read_matrix1D (in_path, &rows, &cols, matrix);
        if (result != 0) {
            fprintf (stderr, "Failed to read matrix from file %s\n", in_path);
            // Free the allocated memory in case of failure
            free (matrix);
            return -1;
        }
        printf ("Matrix read successfully from file %s with dimensions %dx%d, interation %d\n", in_path, rows, cols, iterations);    
    }
    float *p_matrix = (float*)malloc (chunk * cols * sizeof (float));   // Each process has one p_matrix
    float *p_new_matrix = (float*)malloc (chunk * cols * sizeof (float));   // Each process has one p_matrix    
    MPI_Scatter (world_rank == root_rank ? matrix : NULL, chunk * cols, MPI_FLOAT, p_matrix, chunk * cols, MPI_FLOAT, root_rank, MPI_COMM_WORLD);
    if (world_rank == root_rank){
        free (matrix);
    }
    float *list_c_im1_j, *list_c_ip1_j;
    if (world_rank != 0)           // Rank 0 not need to receive list_c_im1_j
        list_c_im1_j = (float*)malloc (cols * sizeof (float));
    
    if (world_rank != world_size - 1)            list_c_ip1_j = (float*)malloc (cols * sizeof (float)); 
    // Rank (world_size - 1) not need to receive list_c_ip1_j    
        list_c_ip1_j = (float*)malloc (cols * sizeof (float)); 
    
    double start_time = MPI_Wtime();
    int num_uncontaminated_count[iterations]; // Hold the number of block uncontaminated
    for (int t = 0; t < iterations; t++){
        int local_uncontaminated_count = 0;
        int global_uncontaminated_count = 0;
        MPI_Request send_rq;
        // Exchange boundary 
        // Send final row to next rank
        if (world_rank != world_size -1)    MPI_Isend ((void*)&p_matrix[(chunk - 1) * cols], 
                                            cols, MPI_FLOAT, world_rank + 1, 0, MPI_COMM_WORLD,&send_rq); // The final row of each block    
        
        // Send first row to previous rank
        if (world_rank != 0)                MPI_Isend ((void*)p_matrix,cols, MPI_FLOAT, 
                                            world_rank - 1, 0, MPI_COMM_WORLD,&send_rq); // The frist row of each block  
        
        if (world_rank != 0){   
            // Rank 0 not need to receive list_c_im1_j
            MPI_Recv (list_c_im1_j, cols, MPI_FLOAT, world_rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
        if (world_rank != world_size - 1){
            // Rank (world_size - 1) not need to receive list_c_ip1_j    
            MPI_Recv (list_c_ip1_j, cols, MPI_FLOAT, world_rank + 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
        for (int i = 0; i < chunk; i++){
            for (int j = 0; j < cols; j++){
                float c_i_j = p_matrix [i * cols + j];
                float c_im1_j = (i - 1 >= 0) ? p_matrix [(i-1)* cols + j] : (world_rank != 0 ? list_c_im1_j[j] : 0.00);
                float c_i_jm1 = j - 1 >= 0  ? p_matrix[i * cols + (j-1)] : 0;
                float c_ip1_j = i + 1 < chunk ? p_matrix[(i + 1) * cols + j] : (world_rank != world_size - 1 ? list_c_ip1_j[j] : 0.00);
                float c_i_jp1 = j + 1 < cols ? p_matrix[i * cols + (j + 1)] : 0;
                // Step 1: Caculate the advection 
                float advec = calculate_advection (c_i_j, c_i_jm1, c_im1_j, ux, uy, dx, dy);

                // Step 2: Caculate the diffusion
                float diffu = calculate_diffusion (c_i_j, c_ip1_j, c_im1_j, c_i_jp1, c_i_jm1, D, dx, dy);

                // Step 3: Caculate the decay
                float decay = calculate_decay (c_i_j, lamda, k);

                // Step 4: Caculate C new
                float c_new = update_concentration (c_i_j, (advec + diffu + decay), dt);
                // Requirement: Count uncontaminated blocks (assuming == 0.0 is uncontaminated)
                if (c_new == 0.00) {
                    local_uncontaminated_count++;
                }
                if (c_new  < 0.00 ){
                    fprintf (stderr, "Return error value from pde function\n");
                }
                p_new_matrix[i * cols + j] = c_new;
            }
        }
        // --- C. AGGREGATE RESULTS (MPI_Reduce) and SYNCHRONIZE ---
        
        // MPI_Reduce: Sum local counts to root (Rank 0)
        MPI_Reduce(&local_uncontaminated_count, &global_uncontaminated_count, 1, MPI_INT, MPI_SUM, root_rank, MPI_COMM_WORLD);

        // Debug output for the count (only on root)
        num_uncontaminated_count[t] = global_uncontaminated_count;
        // 4. SWAP: After calculating ALL new values, the new matrix becomes the old matrix for the next iteration
        float *temp = p_matrix;
        p_matrix = p_new_matrix;
        p_new_matrix = temp;
        MPI_Barrier (MPI_COMM_WORLD); // Barrier before continue next iterations
    }
    if (world_rank != 0){
        free (list_c_im1_j);
    }
    if (world_rank != world_size - 1){
        free (list_c_ip1_j);
    }
    if (world_rank == root_rank){
        finished = 1;
    }
    MPI_Bcast ((void*) &finished, 1, MPI_INT, root_rank, MPI_COMM_WORLD);   // Stop signal
    if (finished){
        printf ("Process %d receive stop signal\n",world_rank);
        if (world_rank == root_rank){
            matrix = (void*)malloc (chunk * world_size * cols *  sizeof (float));
        }
        MPI_Gather (p_matrix, chunk * cols, MPI_FLOAT, world_rank == root_rank ? (void*) matrix : NULL, chunk * cols, MPI_FLOAT, root_rank, MPI_COMM_WORLD);
        // Free on each process
        free (p_matrix);
        free (p_new_matrix);
        if (world_rank != root_rank){
            // End all of process not root
            MPI_Finalize();
            return 0;   
        }
    }
    // Endtime 
    double end_time = MPI_Wtime();
    printf ("Parallel simulation finished in %.4f seconds\n", end_time - start_time);
    for (int i = 0; i < iterations; i++){
        printf("Time step %d: Total Uncontaminated Blocks = %d\n", i, num_uncontaminated_count[i]); // Print the number of block is uncontaminated blocks
    }
    // 5. Write result to output file and free allocated memory
    write_matrix1D (out_path, rows, cols, matrix);    
    free (matrix);
    MPI_Finalize();
    return 0;
}