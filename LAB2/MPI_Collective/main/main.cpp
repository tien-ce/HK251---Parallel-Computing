#include <mpi.h>
#include <stdio.h>
#include <vector>
#include <numeric>
#include <cstdlib>
#include <algorithm>
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
    int N = 16; // Defaule value *problem size)
    if (world_rank == 0 && argc > 1){
        // Argv from user argument
        N = std::max (1, atoi(argv[1]));   
    }
    MPI_Bcast ((void*)&N, 1, MPI_INT, 0, MPI_COMM_WORLD); // Broadcast from root ranking 0

    // Partition with padding so Scatter can send equal chunks
    int chunk = (N + world_size - 1) / world_size; //(Ceil (N/world_size)) - The size of portion data each process will receive 
    int padded = chunk * world_size; // the total size of the array after adding virtual "padding" elements to make the size perfectly divisible by world_size.
    
    /*------------------- Prepare data only on root ranking 0 -------------------------*/ 
    std::vector<int> sendbuf; // only valid on root
    if (world_rank == 0){
        sendbuf.resize (padded,0); // Padded is the size perfectly divisible by world_size
        std::iota (sendbuf.begin(),sendbuf.begin()+N,1); // Initialize correspondinglly value for array 1....N and stay 0 for padding (padded - N)
    }
    /*------------------- Barrier: Synchronize before timing ----------------*/
    MPI_Barrier (MPI_COMM_WORLD);
    double t0 = MPI_Wtime();
    std::vector<int> local (chunk,0); // Initialize vector for each process with the size is chunk and initial value is 0
    MPI_Scatter (world_rank == 0 ? (void*)sendbuf.data() : nullptr, chunk, MPI_INT, (void*)local.data(), chunk, MPI_INT, 0, MPI_COMM_WORLD);

    /*------------------- Local work : Compute local sum --------------------*/
    int local_sum = std::accumulate(local.begin(),local.end(),0);

    /*------------------- Allreduce: Global sum on every rank ---------------------*/
    int global_sum_all = 0;
    MPI_Allreduce ((void*)&local_sum, &global_sum_all, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);

    /*-------------------- Reduce: Global sum only on root*/
    int global_sum_root = 0;
    MPI_Reduce ((void*)&local_sum, (void*)&global_sum_root, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    /*------------------- Gather: Collect each rank's partial sum ---------------------*/
    std::vector<int> partial_sums; // Only meaningfule on root ranking 0
    if (world_rank == 0){
        partial_sums.resize (world_size);
    }
    MPI_Gather ((void*)&local_sum, 1, MPI_INT, world_rank == 0 ? (void*)(partial_sums.data()) : nullptr, 1, MPI_INT, 0, MPI_COMM_WORLD);

    /*------------------- Barrier: Synchronize end of the pharse ---------------------*/
    MPI_Barrier (MPI_COMM_WORLD);
    double t1 = MPI_Wtime();

    /*------------------- Report on root --------------------*/
    if (world_rank == 0){
        long long expected = 1LL * N * (N+1) / 2;
        std::cout << "N = " << N << ", world_size " << world_size << std::endl;
        std::cout << "Per-rank partial sums: ";
        for (int s: partial_sums) std::cout << s << "\t";
        std::cout << "\n Allreduce global sum = " << global_sum_all
                  << " | Reduce (to root) = " << global_sum_root
                  << " | Expected = " << expected << std::endl;
        std::cout << ((global_sum_all == expected && expected == global_sum_root) ? "OK: Sums match." : "Warning: Mismatch") << std::endl;
        std::cout << "Elapsed (synchronized) time = " << (t1-t0) << "s" << std::endl;
    }
    
    MPI_Finalize();
    return 0;
}