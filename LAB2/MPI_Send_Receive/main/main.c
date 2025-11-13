#include <mpi.h>
#include <stdio.h>
int main(int argc, char** argv) {
    MPI_Init (&argc,&argv);
    int world_rank = -1, world_size = 0;
    MPI_Comm_rank (MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size (MPI_COMM_WORLD, &world_size);
    if (world_size <= 1){
        if (world_rank == 0){
        fprintf(stderr, "Need at least 2 processes for this communication logic.\n");
        }
        MPI_Abort(MPI_COMM_WORLD, MPI_ERR_ARG);
    }
    if (world_rank == 0){
        int temp_c = 27;
        // Synchronous Send: completes only when Rank 1 has posted its matching receive
        MPI_Ssend ((void*)(&temp_c), 1, MPI_INT, 1, 100, MPI_COMM_WORLD);
        printf ("Rank 0 sen tempurature %d C to Rank 1 (synchronous)\n",temp_c);
    }
    else if (world_rank == 1){
        int received = -1;
        MPI_Recv ((void*)(&received),1,MPI_INT,0,100,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
        printf("Rank 1 received temperature %d C from Rank 0\n", received);   
    }
    else{
        printf ("Hello from other rank: Rank %d\n",world_rank);
    }
    MPI_Finalize();
    return 0;
}